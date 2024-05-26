#include "BGSubtractor.h"

uchar BGSubtractor::findMedianValue(vector<uchar>& Buff) {
    size_t medianNum = Buff.size() / 2;
    nth_element(Buff.begin(), Buff.begin() + medianNum, Buff.end());
    return Buff[medianNum];
}

void BGSubtractor::Initialization(Mat& frame) {

    medianFrame = Mat::zeros(frame.size(), frame.type());

    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            vector<uchar> pixelsB, pixelsG, pixelsR;

            for (Mat buffer : imageBuffer)
            {
                Vec3b pixel = buffer.at<Vec3b>(i, j);
                pixelsB.push_back(pixel[0]);
                pixelsG.push_back(pixel[1]);
                pixelsR.push_back(pixel[2]);
            }

            medianFrame.at<Vec3b>(i, j)[0] = findMedianValue(pixelsB);
            medianFrame.at<Vec3b>(i, j)[1] = findMedianValue(pixelsG);
            medianFrame.at<Vec3b>(i, j)[2] = findMedianValue(pixelsR);

            if (imageBuffer.size() == (bufferSize - 1))
            {
                bufferPixelsB.push_back(pixelsB);
                bufferPixelsG.push_back(pixelsG);
                bufferPixelsR.push_back(pixelsR);
            }
        }
    }
}

void BGSubtractor::updateBuffer(vector<vector<uchar>>& bufferPixels, Mat& frame, int i, int j, int channel){

    bufferPixels[i * frame.cols + j].erase(bufferPixels[i * frame.cols + j].begin());

    bufferPixels[i * frame.cols + j].push_back(imageBuffer[bufferSize - 1].at<Vec3b>(i, j)[channel]);

    medianFrame.at<Vec3b>(i, j)[channel] = findMedianValue(bufferPixels[i * frame.cols + j]);
}

void BGSubtractor::updateBackground(Mat& frame) {
    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            updateBuffer(bufferPixelsB, frame, i, j, 0);
            updateBuffer(bufferPixelsG, frame, i, j, 1);
            updateBuffer(bufferPixelsR, frame, i, j, 2);
        }
    }
}

Mat BGSubtractor::applyPrewittEdge(Mat& frame) {

    Mat prewittX = (Mat_<char>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    Mat prewittY = (Mat_<char>(3, 3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);

    Mat grayFrame;
    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);

    Mat gradX, gradY;
    filter2D(grayFrame, gradX, CV_16S, prewittX);
    filter2D(grayFrame, gradY, CV_16S, prewittY);

    Mat absGradX, absGradY;
    convertScaleAbs(gradX, absGradX);
    convertScaleAbs(gradY, absGradY);

    Mat edgeImage;
    addWeighted(absGradX, 0.5, absGradY, 0.5, 0, edgeImage);

    return edgeImage;
}

Mat BGSubtractor::getForeground(Mat& frame) {

    Mat foreground;
    absdiff(frame, medianFrame, foreground);

    Mat grayForeground;
    cvtColor(foreground, grayForeground, COLOR_BGR2GRAY);

    Mat binaryForeground;
    threshold(grayForeground, binaryForeground, 50, 255, THRESH_BINARY);

    return binaryForeground;
}

Mat BGSubtractor::applyMorphologicalOperations(Mat& FramePrewitt, Mat& BackgroundPrewitt)
{
    Mat subtractionImage;
    subtract(FramePrewitt, BackgroundPrewitt, subtractionImage);

    Mat thresholdedImage;
    threshold(subtractionImage, thresholdedImage, 50, 255, THRESH_BINARY);


    dilate(thresholdedImage, thresholdedImage, getStructuringElement(MORPH_RECT, Size(5, 5)));
    morphologyEx(thresholdedImage, thresholdedImage, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(3, 3)));
    erode(thresholdedImage, thresholdedImage, getStructuringElement(MORPH_RECT, Size(6, 6)));
    morphologyEx(thresholdedImage, thresholdedImage, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(7, 7)));

    return thresholdedImage;
}

Mat BGSubtractor::findCenter(Mat& thresholdedFrame, Mat& frame) {

    Mat resultImage;
    frame.copyTo(resultImage);
    vector<vector<Point>> contours;
    findContours(thresholdedFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        Moments m = moments(contours[i], false);
        Point2f centroid(m.m10 / m.m00, m.m01 / m.m00);
        cout << "Centroid of contour " << i << " is at (" << centroid.x << ", " << centroid.y << ")" << endl;

        circle(resultImage, centroid, 5, Scalar(0, 0, 255), -1);
    }

    //string text = "Number of Object: " + to_string(contours.size());

    //putText(resultImage, text, Point(10,10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    return resultImage;
}

void BGSubtractor::getBackground(Mat& frame, string FolderPath, bool FG) {

    counter++;

    imageBuffer.push_back(frame);

    if (imageBuffer.size() > bufferSize)
    {
        imageBuffer.erase(imageBuffer.begin());
    }

    if (imageBuffer.size() < bufferSize)
    {
        Initialization(frame);
    }
    
    if (imageBuffer.size() == bufferSize)
    {
        updateBackground(frame);
    }

    if(FG)
    {
      Mat prewittFrame = applyPrewittEdge(frame);
      Mat prewittMedian = applyPrewittEdge(medianFrame);
      Mat thresholdedImage = applyMorphologicalOperations(prewittFrame, prewittMedian);
      Mat resultImage = findCenter(thresholdedImage, frame);
      imwrite(FolderPath + "/FG_Edge/" + to_string(counter) + ".jpg", thresholdedImage);
      imwrite(FolderPath + "/Result/" + to_string(counter) + ".jpg", resultImage);
    }
    else{
      Mat foreground = getForeground(frame);
      imwrite(FolderPath + "/FG/" + to_string(counter) + ".jpg", foreground);
    }

    
    imwrite(FolderPath + "/BG/" + to_string(counter) + ".jpg", medianFrame);
    imwrite(FolderPath + "/Frame/" + to_string(counter) + ".jpg", frame);;
}