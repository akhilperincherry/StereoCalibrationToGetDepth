#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

int main(void)
{

// Mat mat_img_left = imread("/home/Akhil/codes/UndistortUsingStereoCalib/left_out_N7test.jpg");
// Mat mat_img_right = imread("/home/Akhil/codes/UndistortUsingStereoCalib/right_out_N7test.jpg");

Mat mat_img_left = imread("/home/Akhil/codes/UndistortUsingStereoCalib/frame_1l.png");
Mat mat_img_right = imread("/home/Akhil/codes/UndistortUsingStereoCalib/frame_1r.png");

int stepSize = 40;

int width = mat_img_left.size().width;
int height = mat_img_left.size().height;

for (int i = 0; i<height; i += stepSize){
    cv::line(mat_img_left, Point(0, i), Point(width, i), cv::Scalar(0, 255, 255));
    cv::line(mat_img_right, Point(0, i), Point(width, i), cv::Scalar(0, 255, 255));

}

//for (int i = 0; i<width; i += stepSize)
    //cv::line(mat_img, Point(i, 0), Point(i, height), cv::Scalar(255, 0, 255));

imshow("Left",mat_img_left);
imshow("Right",mat_img_right);
imwrite("LeftRectified.png", mat_img_left);
imwrite("RightRectified.png", mat_img_right);


waitKey();
return 0;
}