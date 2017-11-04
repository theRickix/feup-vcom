#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <algorithm>
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
	vector<Point> potentialLines,finalLines;
	int hour=-1,seconds=-1,minutes=-1;
	double max=-1, middle=-1, min=-1;
	// detect the lines
	HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
	for( size_t i = 0; i < lines.size(); i++ )	{
		Vec4i l = lines[i];
		Point begin(l[0],l[1]), end(l[2],l[3]);
		//check if is potential line by checking if it is near the center
		if(abs(distanceToPoint(center,begin,end))<3) {
			//guess which "final" point of the line is (not the center)
			if(distanceBetweenPoints(center,begin) > distanceBetweenPoints(center,end))
				potentialLines.push_back(begin);
			else
				potentialLines.push_back(end);
		}
	}

	cout << potentialLines.size() << "\n";

	/*for( size_t i = 0; i < potentialLines.size(); i++ )
			line( cdst, center, potentialLines[4], Scalar(0,0,255), 3, CV_AA);*/

	while(!potentialLines.empty() && finalLines.size()<3){
		Point p1 = potentialLines.back();
		potentialLines.pop_back();

		vector<Point> similar;

		for(size_t i=0; i<potentialLines.size(); i++) {
			Point p2 = potentialLines[i];
			double gradient1 = (p1.y-center.y)/(p1.x-center.x);
			double gradient2 = (p2.y-center.y)/(p2.x-center.x);
			if(gradient1 == gradient2 && distanceBetweenPoints(p1,p2)<=30) {
				similar.push_back(p2);
				potentialLines.erase(potentialLines.begin()+i);
			}
		}

		if(similar.size() > 0) {
			int biggestX=0, biggestY=0,size=0;
			for(size_t i=0; i<similar.size(); i++) {
				if(distanceBetweenPoints(center,similar[i])>size) {
					biggestX = similar[i].x;
					biggestY = similar[i].y;
				}
			}
			finalLines.push_back(Point(biggestX,biggestY));
		}
		else
			finalLines.push_back(p1);
	}

	//check for "duplicate" lines
	while(!potentialLines.empty() && finalLines.size()<3){
		Point p1 = potentialLines.back();
		potentialLines.pop_back();
		Point p2 = potentialLines.back();
		potentialLines.pop_back();
		double gradient1 = (p1.y-center.y)/(p1.x-center.x);
		double gradient2 = (p2.y-center.y)/(p2.x-center.x);
		if(gradient1 == gradient2 && distanceBetweenPoints(p1,p2)<=30) {
			finalLines.push_back(Point((p1.x+p2.x)/2,(p1.y+p2.y)/2));
		}
		else {
			potentialLines.push_back(p2);
			finalLines.push_back(p1);
		}
	}

	cout << finalLines.size() << "\n";

	//sort
	Point swap;
	for (int c = 0 ; c < ( finalLines.size() - 1 ); c++) {
		for (int d = 0 ; d < finalLines.size() - c - 1; d++) {
			if (distanceBetweenPoints(center,finalLines[d]) > distanceBetweenPoints(center,finalLines[d+1])) { /* For decreasing order use < */

				swap       = finalLines[d];
				finalLines[d]   = finalLines[d+1];
				finalLines[d+1] = swap;
			}
		}
	}


	//line( cdst, center, finalLines[i], Scalar(0,0,255), 3, CV_AA);

	//Draw each pointer
	line( cdst, center, finalLines[0], Scalar(255,0,0), 3, CV_AA);
	line( cdst, center, finalLines[1], Scalar(0,255,0), 3, CV_AA);
	if(finalLines.size()==3) line( cdst, center, finalLines[2], Scalar(0,0,255), 3, CV_AA);

	//calculate angles
	int hourAngle = getAngle(finalLines[0],center);
	cout << "Hour: " << (hourAngle)%360/30 << "\n";

	int minutesAngle = getAngle(finalLines[1],center);
	cout << "Minutes: " << (minutesAngle)%360/6 << "\n";

	if(finalLines.size() == 3) {
		int secondsAngle = getAngle(finalLines[2],center);
		cout << "Seconds: " << (secondsAngle)%360/6 << "\n";
	}



	imshow("source", src);
	imshow("detected lines", cdst);
	waitKey();

	return 0;
}
