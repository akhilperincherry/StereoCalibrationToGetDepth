#include <opencvec2/core/core.hpp>
#include <opencvec2/calib3d/calib3d.hpp>
#include <opencvec2/highgui/highgui.hpp>
#include <opencvec2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

vector< vector< Point3f > > objEachPoints;
vector< vector< Point2f > > imgPoints1, imgPoints2;
vector< Point2f > corners1, corners2;
vector< vector< Point2f > > left_imgPoints, right_imgPoints;

Mat img1, img2, img1_gray, img2_gray;

void load_image_points(int board_width, int board_height, int num_imgs, float square_size,
                      char* leftimg_dir, char* rightimg_dir, char* leftimgSuffix, char* rightimgSuffix) {

  Size board_size = Size(board_width, board_height);
  int board_n = board_width * board_height;

  for (int i = 1; i <= num_imgs; i++) {
    char left_img[100], right_img[100];
    sprintf(left_img, "%s%s%d.jpg", leftimg_dir, leftimgSuffix, i);
    sprintf(right_img, "%s%s%d.jpg", rightimg_dir, rightimgSuffix, i);
    img1 = imread(left_img, CV_LOAD_IMAGE_COLOR);
    img2 = imread(right_img, CV_LOAD_IMAGE_COLOR);
    cvtColor(img1, img1_gray, CV_BGR2GRAY);
    cvtColor(img2, img2_gray, CV_BGR2GRAY);

    bool found1 = false, found2 = false;

    found1 = cv::findChessboardCorners(img1, board_size, corners1,
  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
    found2 = cv::findChessboardCorners(img2, board_size, corners2,
  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

    if (found1)
    {
      cv::cornerSubPix(img1_gray, corners1, cv::Size(5, 5), cv::Size(-1, -1),
  cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
      cv::drawChessboardCorners(img1_gray, board_size, corners1, found1);
    }
    if (found2)
    {
      cv::cornerSubPix(img2_gray, corners2, cv::Size(5, 5), cv::Size(-1, -1),
  cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
      cv::drawChessboardCorners(img2_gray, board_size, corners2, found2);
    }

    vector< Point3f > objEach;
    for (int i = 0; i < board_height; i++)
      for (int j = 0; j < board_width; j++)
        objEach.push_back(Point3f((float)j * square_size, (float)i * square_size, 0));

    if (found1 && found2) {
      cout << i << ". Found corners!" << endl;
      imgPoints1.push_back(corners1);
      imgPoints2.push_back(corners2);
      objEachPoints.push_back(objEach);
    }
  }
  for (int i = 0; i < imgPoints1.size(); i++) {
    vector< Point2f > vec1, vec2;
    for (int j = 0; j < imgPoints1[i].size(); j++) {
      vec1.push_back(Point2f((double)imgPoints1[i][j].x, (double)imgPoints1[i][j].y));
      vec2.push_back(Point2f((double)imgPoints2[i][j].x, (double)imgPoints2[i][j].y));
    }
    left_imgPoints.push_back(vec1);
    right_imgPoints.push_back(vec2);
  }
}

int main(int argc, char const *argv[])
{
  int board_width, board_height;

  char* leftcalib_file = /*"/home/Akhil/codes/Intrinsics/*/"cam_left.yml";
  char* rightcalib_file = /*"/home/Akhil/codes/Intrinsics/*/"cam_right.yml";
  char* leftimg_dir = "/home/Akhil/codes/readEveryNthFrame/dataN7/";
  char* rightimg_dir = "/home/Akhil/codes/readEveryNthFrame/dataN7/";
  char* leftimgSuffix = "left";
  char* rightimgSuffix = "right";
  char* out_file = "cam_stereo.yml";
  char* out_img_file = "cam_image.yml";

  int num_imgs = 72;
  board_width = 7;
  board_height = 5;
  float square_size = 13;

  // board_width = 21;
  // board_height = 21;
  // int num_imgs = 78;
  // float square_size = 0.4;

  FileStorage fsl(leftcalib_file, FileStorage::READ);
  FileStorage fsr(rightcalib_file, FileStorage::READ);

  load_image_points(board_width, board_height, num_imgs, square_size,
                   leftimg_dir, rightimg_dir, leftimgSuffix, rightimgSuffix);

  printf("Starting Stereo Calibration....\n");
  Mat K1, K2, R, F, E;
  Vec3d T;
  Mat D1, D2;
  fsl["K"] >> K1;
  fsr["K"] >> K2;
  fsl["D"] >> D1;
  fsr["D"] >> D2;
  int flag = 0;
  flag |= CV_CALIB_FIX_INTRINSIC;
  
  cout << "Extracting intrinsic Parameters..." << endl;
  
  double rms = stereoCalibrate(objEachPoints, left_imgPoints, right_imgPoints, K1, D1, K2, D2, img1.size(), R, T, E, F);
  cout<<"rms "<<rms<<endl;

  cv::FileStorage fs1(out_file, cv::FileStorage::WRITE);
  fs1 << "K1" << K1;
  fs1 << "K2" << K2;
  fs1 << "D1" << D1;
  fs1 << "D2" << D2;
  fs1 << "R" << R;
  fs1 << "T" << T;
  fs1 << "E" << E;
  fs1 << "F" << F;

  cv::FileStorage Imagepts1(out_img_file, cv::FileStorage::WRITE);
  Imagepts1 << "ImgPointsLeft" << left_imgPoints;
  Imagepts1 << "ImgPointsRight" << right_imgPoints;
  
  printf("Calibration Finished...\n");

  printf("Rectification...\n");

  cv::Mat R1, R2, P1, P2, Q;
  stereoRectify(K1, D1, K2, D2, img1.size(), R, T, R1, R2, P1, P2, Q);

  fs1 << "R1" << R1;
  fs1 << "R2" << R2;
  fs1 << "P1" << P1;
  fs1 << "P2" << P2;
  fs1 << "Q" << Q;

  printf("Finished!\n");

  return 0;
}
