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

class Line {
public:
	Point begin;
	Point end;
	Point final;
public:
	Line() {}
	Line(Point begin1,Point end1, Point center) {
		begin=begin1;
		end=end1;
		if(distanceBetweenPoints(center,begin) > distanceBetweenPoints(center,end))
			final=begin1;
		else
			final=end1;
	}
};

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
	vector<Line> potentialLines,finalLines;
	// detect the lines
	HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
	for( size_t i = 0; i < lines.size(); i++ )	{
		Vec4i l = lines[i];
		Point begin(l[0],l[1]), end(l[2],l[3]);
		//check if is potential line by checking if it is near the center
		if(abs(distanceToPoint(center,begin,end))<3) {
			//guess which "final" point of the line is (not the center)
			potentialLines.push_back(Line(begin,end,center));
		}
	}

	cout << potentialLines.size() << "\n";

	/*for( size_t i = 0; i < potentialLines.size(); i++ )
			line( cdst, center, potentialLines[4], Scalar(0,0,255), 3, CV_AA);*/

	while(!potentialLines.empty() && finalLines.size()<3){
		Line p1 = potentialLines.back();
		potentialLines.pop_back();

		vector<Line> similar;

		for(size_t i=0; i<potentialLines.size(); i++) {
			Line p2 = potentialLines[i];
			double gradient1 = (p1.final.y-center.y)/(p1.final.x-center.x);
			double gradient2 = (p2.final.y-center.y)/(p2.final.x-center.x);
			if(gradient1 == gradient2 && distanceBetweenPoints(p1.final,p2.final)<=30) {
				similar.push_back(p2);
				potentialLines.erase(potentialLines.begin()+i);
			}
		}

		if(similar.size() > 0) {
			similar.push_back(p1);
			int sumX1=0,sumX2=0,sumY1=0,sumY2=0;
			for(size_t i=0; i<similar.size(); i++) {

				sumX1+=similar[i].begin.x;
				sumX2+=similar[i].end.x;
				sumY1+=similar[i].begin.y;
				sumY2+=similar[i].end.y;
			}
			finalLines.push_back(Line(Point(sumX1/similar.size(),sumY1/similar.size()),Point(sumX2/similar.size(),sumY2/similar.size()),center));
		}
		else
			finalLines.push_back(p1);
	}
	cout << finalLines.size() << "\n";

	//sort
	Line swap;
	for (size_t c = 0 ; c < ( finalLines.size() - 1 ); c++) {
		for (size_t d = 0 ; d < finalLines.size() - c - 1; d++) {
			if (distanceBetweenPoints(finalLines[d].begin,finalLines[d].end) > distanceBetweenPoints(finalLines[d+1].begin,finalLines[d+1].end)) { /* For decreasing order use < */

				swap       = finalLines[d];
				finalLines[d]   = finalLines[d+1];
				finalLines[d+1] = swap;
			}
		}
	}


	//line( cdst, center, finalLines[i], Scalar(0,0,255), 3, CV_AA);

	//Draw each pointer
	line( cdst, center, finalLines[0].final, Scalar(255,0,0), 3, CV_AA);
	line( cdst, center, finalLines[1].final, Scalar(0,255,0), 3, CV_AA);
	if(finalLines.size()==3) line( cdst, center, finalLines[2].final, Scalar(0,0,255), 3, CV_AA);

	//calculate angles
	int hourAngle = getAngle(finalLines[0].final,center);
	cout << "Hour: " << (hourAngle)%360/30 << "\n";

	int minutesAngle = getAngle(finalLines[1].final,center);
	cout << "Minutes: " << (minutesAngle)%360/6 << "\n";

	if(finalLines.size() == 3) {
		int secondsAngle = getAngle(finalLines[2].final,center);
		cout << "Seconds: " << (secondsAngle)%360/6 << "\n";
	}



	imshow("source", src);
	imshow("detected lines", cdst);
	waitKey();

	return 0;
}
