#include <iostream>
#include "BGSubtractor.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv) {

    if (argc < 7) {
      cout << "Usage: " << argv[0] << " -video_path <video_path> -folder_path <folder_path> -threshold_value <0-255> -edge" << endl;
      return -1;
    }
    char* videoPath = argv[2];
    VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        cout << "Error opening video file." << endl;
        return -1;
    }

    char* folderPath = argv[4];

    bool edge = false;
    if(argc > 7)
    {
      string edgeFlag = argv[7];
      if(edgeFlag == "-edge")
      {
        edge = true;
      }
    }

    float thresholdValue = atof(argv[6]);

    BGSubtractor BG;

    while (1)
    {
        Mat frame;
        cap >> frame;

        BG.getBackground(frame, folderPath, edge, thresholdValue);

    }

    cap.release();
    destroyAllWindows();
    return 0;
}

