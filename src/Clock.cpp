#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, char** argv ) {
	Mat src;
	src = imread( argv[1], 1 );

	if( argc != 2 || !src.data )
	{
		printf( "No image data \n" );
		return -1;
	}

	Mat dst, cdst;
	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);

	//detect circle
	std::vector<Vec3f> circles;
	HoughCircles(dst,circles,CV_HOUGH_GRADIENT,2,50,200,100,100,300);

	int pos=0;
	int max=-1;
	for(size_t i = 0; i < circles.size(); i++ )
	{
		Vec3f f=circles[i];
		if(f[2]>max && f[0]+f[2]<dst.rows && f[0]-f[2]>=0 && f[1]+f[2]<dst.cols && f[1]-f[2]>0)
		{
			max=f[2];
			pos=i;
		}
	}
	Point center(circles[pos][0],circles[pos][1]);
	int   radius= circles[pos][2];
	circle(cdst,center,radius,Scalar(255),2);

	vector<Vec4i> lines;
	// detect the lines
	HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
		Vec4i l = lines[i];
		// draw the lines
		line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
	}

	imshow("source", src);
	imshow("detected lines", cdst);

	waitKey(0);

	return 0;
}
