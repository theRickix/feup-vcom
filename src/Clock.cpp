#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int getAngle(Point p1, Point p2) {
	double angle = atan2(p1.y - p2.y, p1.x - p2.x);

	//if negative, turn it positive
	if(angle <0)
		angle+=2*CV_PI;

	angle*=180/CV_PI; //to degrees

	//change 0 degress point from right to top
	if(angle <=270)
		angle+=90;
	else
		angle-=270;

	//cout << angle << "\n";
	return angle;
}
double crossProduct(Point a, Point b) {
	return a.x*b.y - a.y*b.x;
}

double dotProduct(Point a1, Point a2, Point b1, Point b2) {
	Point vectorA(a1.x - a2.x, a1.y - a2.y), vectorB(a1.x - a2.x, a1.y - a2.y);
	return vectorA.x * vectorB.x + vectorA.y * vectorB.y;
}

double distanceBetweenPoints(Point begin,Point end) {
	return sqrt(pow(end.x-begin.x,2)+pow(end.y-begin.y,2));
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

	//get only the "final" point of each pointer
	//hour
	Point hourPoint;
	if(distanceBetweenPoints(center,Point(lines[hour][0],lines[hour][1])) > distanceBetweenPoints(center,Point(lines[hour][2],lines[hour][3])))
		hourPoint = Point(lines[hour][0],lines[hour][1]);
	else
		hourPoint = Point(lines[hour][2],lines[hour][3]);

	//minutes
	Point minutesPoint;
	if(distanceBetweenPoints(center,Point(lines[minutes][0],lines[minutes][1])) > distanceBetweenPoints(center,Point(lines[minutes][2],lines[minutes][3])))
		minutesPoint = Point(lines[minutes][0],lines[minutes][1]);
	else
		minutesPoint = Point(lines[minutes][2],lines[minutes][3]);

	//seconds
	Point secondsPoint;
	if(distanceBetweenPoints(center,Point(lines[seconds][0],lines[seconds][1])) > distanceBetweenPoints(center,Point(lines[seconds][2],lines[seconds][3])))
		secondsPoint = Point(lines[seconds][0],lines[seconds][1]);
	else
		secondsPoint = Point(lines[seconds][2],lines[seconds][3]);


	//Draw each pointer
	line( cdst, center, hourPoint, Scalar(255,0,0), 3, CV_AA);
	line( cdst, center, minutesPoint, Scalar(0,255,0), 3, CV_AA);
	line( cdst, center, secondsPoint, Scalar(0,0,255), 3, CV_AA);

	//debug
	circle( cdst, minutesPoint, 1, Scalar(0,255,0), -1, 8, 0 );

	//calculate angles
	int hourAngle = getAngle(hourPoint,center);
	cout << "Hour: " << (hourAngle)%360/30 << "\n";

	int minutesAngle = getAngle(minutesPoint,center);
	cout << "Minutes: " << (minutesAngle)%360/6 << "\n";

	int secondsAngle = getAngle(secondsPoint,center);
	cout << "Seconds: " << (secondsAngle)%360/6 << "\n";


	imshow("source", src);
	imshow("detected lines", cdst);
	waitKey();

	return 0;
}
