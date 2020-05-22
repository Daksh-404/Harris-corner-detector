#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;
Mat src, src_gray;
int thresh = 128;
int max_thresh = 255;
const char* source_window = "Source image";
const char* corners_window = "Corners detected";
void cornerHarris_self(Mat dst, int k)
{
    Mat abs_grad_x, abs_grad_y, x2y2, xy, mtrace;
    Mat x2_derivative, y2_derivative, xy_derivative, x2g_derivative, y2g_derivative, xyg_derivative;
    int scale = 1;
    int delta = 0;
    //Step one
     //to calculate x and y derivative of image we use Sobel function
     //Sobel( srcimage, dstimage, depthofimage -1 means same as input, xorder 1,yorder 0,kernelsize 3, BORDER_DEFAULT);
    Sobel(src_gray, abs_grad_x, CV_32FC1, 1, 0, 3, BORDER_DEFAULT);
    Sobel(src_gray, abs_grad_y, CV_32FC1, 0, 1, 3, BORDER_DEFAULT);

    //calculating other three images in M
    pow(abs_grad_x, 2.0, x2_derivative);
    pow(abs_grad_y, 2.0, y2_derivative);
    multiply(abs_grad_x, abs_grad_y, xy_derivative);

    //step three apply gaussain
    GaussianBlur(x2_derivative, x2g_derivative, Size(7, 7), 2.0, 0.0, BORDER_DEFAULT);
    GaussianBlur(y2_derivative, y2g_derivative, Size(7, 7), 0.0, 2.0, BORDER_DEFAULT);
    GaussianBlur(xy_derivative, xyg_derivative, Size(7, 7), 2.0, 2.0, BORDER_DEFAULT);

    //forth step calculating R with k=0.04
    multiply(x2g_derivative, y2g_derivative, x2y2);
    multiply(xyg_derivative, xyg_derivative, xy);
    pow((x2g_derivative + y2g_derivative), 2.0, mtrace);
    dst = (x2y2 - xy) - k * mtrace;
}
void cornerHarris_demo(int, void*)
{
    double k = 0.04;
    Mat dst = Mat::zeros(src.size(), CV_32FC1);
    cornerHarris_self(dst, k);
    Mat dst_norm, dst_norm_scaled;
    normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    convertScaleAbs(dst_norm, dst_norm_scaled);
    for (int i = 0; i < dst_norm.rows; i++)
    {
        for (int j = 0; j < dst_norm.cols; j++)
        {
            if ((int)dst_norm.at<float>(i, j) > thresh)
            {
                circle(dst_norm_scaled, Point(j, i), 2, Scalar(255,255,255), 1, 8, 0);
            }
        }
    }
    namedWindow(corners_window);
    imshow(corners_window, dst_norm_scaled);
}
int main()
{
    src = imread("house.jpg");
    if (src.empty())
    {
        cout << "Could not open or find the image!\n" << endl;
        return -1;
    }
    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    namedWindow(source_window);
    createTrackbar("Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo);
    imshow(source_window, src);
    cornerHarris_demo(0, 0);
    waitKey(0);
    return 0;
}
