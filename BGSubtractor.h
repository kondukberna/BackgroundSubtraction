#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class BGSubtractor {
	vector<Mat> imageBuffer;
	int bufferSize = 30;
	Mat medianFrame;
	vector<vector<uchar>> bufferPixelsB, bufferPixelsG, bufferPixelsR;
	int counter = 0;
public:
	uchar findMedianValue(vector<uchar>& Buff);
	void Initialization(Mat& frame);
	void updateBuffer(vector<vector<uchar>>& bufferPixels, Mat& frame, int i, int j, int channel);
	Mat applyPrewittEdge(Mat& frame);
	Mat applyMorphologicalOperations(Mat& FramePrewitt, Mat& BackgroundPrewitt, float thresholdvalue);
	Mat findCenter(Mat& thresholdedFrame, Mat& frame);
	void updateBackground(Mat& frame);
	Mat getForeground(Mat& frame, float thresholdvalue);
	void getBackground(Mat& frame, string FolderPath, bool FG, float thresholdvalue);
};
