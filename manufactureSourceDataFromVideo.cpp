#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
  char* imgs_directory = "dataN7/";
  char* extension = "jpg";
  int nImages = 0; 
  int frame_ctr = 0;
  int skip_interval = 15;
  //int im_width, im_height;

  // VideoCapture cap1("/home/Akhil/labgit/Pylon/C++/Camera2_lossy.avi");
  // VideoCapture cap2("/home/Akhil/labgit/Pylon/C++/Camera1_lossy.avi");

  VideoCapture cap1("CameraLeft_lossy.avi");
  VideoCapture cap2("CameraRight_lossy.avi");

  Mat img1, img2;
  
  while (1) {
    cap1 >> img1;
    cap2 >> img2;
    frame_ctr++;
    //resize(img1, img_res1, Size(im_width, im_height));
    //resize(img2, img_res2, Size(im_width, im_height));

     if(img1.empty() || img2.empty())
             break;

    imshow("Left", img1);
    imshow("Right", img2);

    waitKey(20);

    if (frame_ctr%skip_interval == 0) {
      nImages++;
      char filename1[200], filename2[200];
      sprintf(filename1, "%sleft%d.%s", imgs_directory, nImages, extension);
      sprintf(filename2, "%sright%d.%s", imgs_directory, nImages, extension);
      cout << "Saving img pair " << nImages << endl;
      imwrite(filename1, img1);
      imwrite(filename2, img2);
    }
  }
  return 0;
}
