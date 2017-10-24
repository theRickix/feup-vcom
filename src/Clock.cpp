#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

double crossProduct(Point a, Point b) {
	return a.x*b.y - a.y*b.x;
}

double distanceBetweenPoints(Point begin,Point end) {
	return sqrt(pow(begin.x+end.x,2)+pow(begin.y+end.y,2));
}

double distanceToPoint(Point center, Point begin, Point end) {
	end -= begin;
	center -= begin;

	double area = crossProduct(center, end);
	return area / norm(end);
}

int main( int argc, char** argv ) {
	Mat src;
	src = imread( argv[1], 1 ); //Read image file from arg

	//Check if image is loaded
	if( argc != 2 || !src.data )
	{
		printf( "No image loaded. \n" );
		return -1;
	}

	Mat dst, cdst,blur; //Additional image files

	Canny(src, dst, 50, 200, 3);
	GaussianBlur(dst, blur, Size(9, 9), 2, 2 );
	cvtColor(dst, cdst, CV_GRAY2BGR);

	//Add circles

	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	HoughCircles(blur, circles, CV_HOUGH_GRADIENT, 1, blur.rows/8, 200, 100, 0, 0 );
	int max_radius=-1, circle_clock=0;

	/// Detect circle clock
	for( size_t i = 0; i < circles.size(); i++ ) {
		if(circles[i][2] > max_radius) {
			circle_clock = i;
			max_radius = circles[i][2];
		}

	}

	//Get center point + radius
	Point center(cvRound(circles[circle_clock][0]), cvRound(circles[circle_clock][1]));
	int radius = cvRound(circles[circle_clock][2]);

	//Draw outline and center
	circle( cdst, center, 3, Scalar(0,255,0), -1, 8, 0 );
	circle( cdst, center, radius, Scalar(0,0,255), 3, 8, 0 );

	vector<Vec4i> lines;
	int hour=-1,seconds=-1,minutes=-1;
	double max=-1, middle=-1, min=-1;
	// detect the lines
	HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
		Vec4i l = lines[i];
		Point begin(l[0],l[1]), end(l[2],l[3]);
		if(abs(distanceToPoint(center,begin,end))<3) {
			double distance = distanceBetweenPoints(begin,end);
			if(distance > max) {
				double tempd = max;
				max = distance;
				min = middle;
				middle = tempd;

				int tempi = seconds;
				seconds = i;
				hour = minutes;
				minutes = tempi;

			}
			else if (distance > middle) {
				double tempd = middle;
				middle = distance;
				min = tempd;

				int tempi = minutes;
				minutes = i;
				hour = tempi;
			}
			else if (distance > min) {
				min = distance;
				hour = i;
			}

			//line( cdst, begin, end, Scalar(0,0,255), 3, CV_AA);
			//cout << distanceBetweenPoints(begin,end) << "\n";
		}

	}

	//cout << hour << " - " << minutes << " - " << seconds << "\n";
	line( cdst, Point(lines[hour][0],lines[hour][1]), Point(lines[hour][2],lines[hour][3]), Scalar(255,0,0), 3, CV_AA);
	line( cdst, Point(lines[minutes][0],lines[minutes][1]), Point(lines[minutes][2],lines[minutes][3]), Scalar(0,255,0), 3, CV_AA);
	line( cdst, Point(lines[seconds][0],lines[seconds][1]), Point(lines[seconds][2],lines[seconds][3]), Scalar(0,0,255), 3, CV_AA);
	imshow("source", src);
	imshow("detected lines", cdst);
	waitKey();

	return 0;
}
