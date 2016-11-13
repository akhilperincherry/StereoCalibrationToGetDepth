#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

vector< vector< Point3f > > objEachPoints;
vector< vector< Point2f > > imgPoints;
vector< Point2f > corners;

Mat img, img_gray;
Size img_size;

void setup_calibration(int board_width, int board_height, int num_imgs, 
                       float square_size, char* img_directory, char* stereoIdentifier,
                       char* extension) {
  Size board_size = Size(board_width, board_height);
  int numSquares = board_width * board_height;

  for (int k = 1; k <= num_imgs; k++) {
    char img_fileName[100];
    sprintf(img_fileName, "%s%s%d.%s", img_directory, stereoIdentifier, k, extension);
    cout<<img_fileName<<endl;
    img = imread(img_fileName, CV_LOAD_IMAGE_COLOR);
    cv::cvtColor(img, img_gray, CV_BGR2img_gray);

    bool found = false;
    found = cv::findChessboardCorners(img, board_size, corners,
                                      CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
    if (found)
    {
      cornerSubPix(img_gray, corners, cv::Size(5, 5), cv::Size(-1, -1),
                   TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
      //drawChessboardCorners(img_gray, board_size, corners, found);
    }
    
    vector< Point3f > objEach;
    for (int i = 0; i < board_height; i++){
      for (int j = 0; j < board_width; j++){
        objEach.push_back(Point3f((float)j * square_size, (float)i * square_size, 0));
      }
    }

    if (found) {
      cout <<  "Found corners " << k << endl;
      imgPoints.push_back(corners);
      objEachPoints.push_back(objEach);
    }
  }
}

int main()
{
  int board_width, board_height, num_imgs;
  //char* img_directory = "/home/Akhil/codes/readVideoSaveFrames/data1/";
  char* img_directory = "/home/Akhil/codes/readEveryNthFrame/dataN7/";
  char* stereoIdentifier = "right";
  char* out_file = "cam_right.yml";
  char* extension = "jpg";


  board_width = 7;
  board_height = 5;
  num_imgs = 72;
  float square_size = 13;

  // board_width = 21;
  // board_height = 21;
  // num_imgs = 78;
  // float square_size = 0.4;

  setup_calibration(board_width, board_height, num_imgs, square_size,
                   img_directory, stereoIdentifier, extension);

  printf("Starting Intrinsic Calibration....\n");
  Mat K;
  Mat D;
  vector< Mat > rvecs, tvecs;
  int flag = 0;
  flag |= CV_CALIB_FIX_K4;
  flag |= CV_CALIB_FIX_K5;
  calibrateCamera(objEachPoints, imgPoints, img.size(), K, D, rvecs, tvecs, flag);

  FileStorage fs(out_file, FileStorage::WRITE);
  fs << "K" << K;
  fs << "D" << D;
  fs << "board_width" << board_width;
  fs << "board_height" << board_height;
  fs << "square_size" << square_size;
  printf("Finished Calibration!\n");

  return 0;
}
