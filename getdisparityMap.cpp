#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utility.hpp"

using namespace cv;
using namespace std;

const char *windowDisparity = "Disparity";
string point_cloud_filename = "";//"pcl";

static void saveXYZ(const char* filename, const Mat& mat)
{
    const double max_z = 1.0e4;
    FILE* fp = fopen(filename, "wt");
    for(int y = 0; y < mat.rows; y++)
    {
        for(int x = 0; x < mat.cols; x++)
        {
            Vec3f point = mat.at<Vec3f>(y, x);
            if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
            fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
        }
    }
    fclose(fp);
}

int main()
{

  // Mat imgLeft = imread("/home/Akhil/codes/UndistortUsingStereoCalib/left_out_N7test.jpg");
  // Mat imgRight = imread("/home/Akhil/codes/UndistortUsingStereoCalib/right_out_N7test.jpg");

  Mat imgLeft = imread("frame_1l.png");
  Mat imgRight = imread("frame_1r.png");

    imshow( "Left", imgLeft );

  
  cvtColor(imgLeft, imgLeft, CV_BGR2GRAY);
  cvtColor(imgRight, imgRight, CV_BGR2GRAY);

  Mat imgDisparity16S = Mat( imgLeft.rows, imgLeft.cols, CV_16SC1 );
  Mat imgDisparity8U = Mat( imgLeft.rows, imgLeft.cols, CV_8UC1 );

  if( imgLeft.empty() || imgRight.empty() )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //Call the constructor for StereoBM
  int ndisparities = 16*5;   /**< Range of disparity */
  int SADWindowSize = 21; /**< Size of the block window. Must be odd */

  Ptr<StereoBM> sbm = StereoBM::create( ndisparities, SADWindowSize );

  //Calculate the disparity image
  sbm->compute( imgLeft, imgRight, imgDisparity16S );

  //Check its extreme values
  double minVal; double maxVal;

  minMaxLoc( imgDisparity16S, &minVal, &maxVal );

  printf("Min disp: %f Max value: %f \n", minVal, maxVal);

  //Display it as a CV_8UC1 image
  imgDisparity16S.convertTo( imgDisparity8U, CV_8UC1, 255/(maxVal - minVal));
  //imgDisparity16S.convertTo( imgDisparity8U, CV_8U);

  namedWindow( windowDisparity, WINDOW_NORMAL );
  imshow( windowDisparity, imgDisparity8U );

  imwrite("SBM_sample.png", imgDisparity16S);
  imwrite("DisparityOutput.png", imgDisparity8U);

  //Depth
  Mat Q;
  FileStorage fs("/home/Akhil/codes/stereoCalibFromImages/cam_stereo.yml", FileStorage::READ);
  fs["Q"] >> Q;
  fs.release();

  //Q.convertTo(Q, CV_32FC1);
  Mat depth(imgLeft.rows, imgLeft.cols, CV_32FC3); 
  //Mat depth;
  //void reprojectImageTo3D(InputArray disparity, OutputArray _3dImage, InputArray Q, bool handleMissingValues=false, int ddepth=-1 ) || ddepth can also be set to CV_16S, CV_32S or CV_32F
  reprojectImageTo3D(imgDisparity8U, depth, Q, false, -1);
  imshow("depth", depth);

  if(!point_cloud_filename.empty())
    {
        printf("storing the point cloud...");
        fflush(stdout);
        Mat depthSave;
        reprojectImageTo3D(imgDisparity8U, depthSave, Q, true);
        saveXYZ(point_cloud_filename.c_str(), depthSave);
        printf("\n");
    }

  waitKey(0);

  return 0;
}

