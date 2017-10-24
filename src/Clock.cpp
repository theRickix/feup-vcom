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

	//todo add circles

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
