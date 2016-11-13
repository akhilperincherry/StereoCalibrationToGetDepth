#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[])
{
  // char* leftimg_filename = "/home/Akhil/codes/readEveryNthFrame/dataN7/left64.jpg";
  // char* rightimg_filename = "/home/Akhil/codes/readEveryNthFrame/dataN7/right64.jpg";
  char* leftimg_filename = "/home/Akhil/codes/readVideoSaveFrames/dataTestN7/left1.jpg";
  char* rightimg_filename = "/home/Akhil/codes/readVideoSaveFrames/dataTestN7/right1.jpg";
  char* calib_file = /*"/home/Akhil/codes/stereoCalibFromImages/*/"cam_stereo.yml";
  char* leftout_filename = "left_out_N7test.jpg";
  char* rightout_filename = "right_out_N7test.jpg";

  Mat R1, R2, P1, P2, Q;
  Mat K1, K2, R;
  Vec3d T;
  Mat D1, D2;
  Mat img1 = imread(leftimg_filename, CV_LOAD_IMAGE_COLOR);
  Mat img2 = imread(rightimg_filename, CV_LOAD_IMAGE_COLOR);
  imshow("Left",img1);
  imshow("Right",img2);

  cv::FileStorage fs1(calib_file, cv::FileStorage::READ);
  fs1["K1"] >> K1;
  fs1["K2"] >> K2;
  fs1["D1"] >> D1;
  fs1["D2"] >> D2;
  fs1["R"] >> R;
  fs1["T"] >> T;

  fs1["R1"] >> R1;
  fs1["R2"] >> R2;
  fs1["P1"] >> P1;
  fs1["P2"] >> P2;
  fs1["Q"] >> Q;

  cv::Mat lmapx, lmapy, rmapx, rmapy;
  cv::Mat imgU1, imgU2;

  cv::initUndistortRectifyMap(K1, D1, R1, P1, img1.size(), CV_32F, lmapx, lmapy);
  cv::initUndistortRectifyMap(K2, D2, R2, P2, img2.size(), CV_32F, rmapx, rmapy);

  cv::remap(img1, imgU1, lmapx, lmapy, cv::INTER_LINEAR);
  cv::remap(img2, imgU2, rmapx, rmapy, cv::INTER_LINEAR);
  
  imwrite(leftout_filename, imgU1);
  imwrite(rightout_filename, imgU2);

  imshow("LeftMapped",imgU1);
  imshow("RightMapped",imgU2);

  waitKey(0);

  return 0;
}
