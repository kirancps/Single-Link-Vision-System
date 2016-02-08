// VisionControl.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include<time.h>
#include <conio.h>
#include "oldaapi.h" //For Keithley
#include <USD_USB.h> //For USDigital Encoder
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <vector>
#include <math.h>

#include "Fruit.h"

#define pi 3.14159
float Ts = 0.028;

#define NZEROS 2
#define NPOLES 2


int deg;
//vector <Fruit> apples;
static float xv[NZEROS + 1] = { 0,0,0 };
static float yv[NPOLES + 1] = { 0,0,0 };

static float xv1[NZEROS + 1] = { 0,0,0 };
static float yv1[NPOLES + 1] = { 0,0,0 };
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed





}

/*
#define GAIN   4.840925170e+00
//fc=20Hz, Fs=80Hz
static float filterloop(float x)
{
float y;
//for (;;)
{
xv[0] = xv[1]; xv[1] = xv[2];
xv[2] = x / GAIN;
yv[0] = yv[1]; yv[1] = yv[2];
yv[2] = (xv[0] + xv[2]) + 2 * xv[1]
+ (-0.1958157127 * yv[0]) + (0.3695273774 * yv[1]);
y = yv[2];
}
return y;
}


static float filter(float x)
{
float y;
//for (;;)
{
xv1[0] = xv1[1]; xv1[1] = xv1[2];
xv1[2] = x / GAIN;
yv1[0] = yv1[1]; yv1[1] = yv1[2];
yv1[2] = (xv1[0] + xv1[2]) + 2 * xv1[1]
+ (-0.1958157127 * yv1[0]) + (0.3695273774 * yv1[1]);
y = yv1[2];
}
return y;
}*/


//fc=40Hz Fs=100Hz
#define GAIN   1.565078650e+00
static float filterloop(float x)
{
	float y;
	{
		xv[0] = xv[1]; xv[1] = xv[2];
		xv[2] = x / GAIN;
		yv[0] = yv[1]; yv[1] = yv[2];
		yv[2] = (xv[0] + xv[2]) + 2 * xv[1]
			+ (-0.4128015981 * yv[0]) + (-1.1429805025 * yv[1]);
		y = yv[2];
	}
	return y;
}
static float filter(float x)
{
	float y;
	{
		xv1[0] = xv1[1]; xv1[1] = xv1[2];
		xv1[2] = x / GAIN;
		yv1[0] = yv1[1]; yv1[1] = yv1[2];
		yv1[2] = (xv1[0] + xv1[2]) + 2 * xv1[1]
			+ (-0.4128015981 * yv1[0]) + (-1.1429805025 * yv1[1]);
		y = yv1[2];
	}
	return y;
}



//using namespace System;
//using namespace System::Windows::Forms;
float Derivative(float e1, float e0)
{
	float e_deri;
	e_deri = (e1 - e0) / Ts;
	return e_deri;
}





int NumberOfUSB1s = 0;
unsigned char ModuleAddress;
bool blnResult;
DBL volt = 2.5, gain, min0, max0, gain1, min1, max1;
HDEV hDev = NULL;
HDASS hDA = NULL, hDA1 = NULL;
LNG iValue;
UINT resolution, encoding, resolution1, encoding1, channel[2] = { 0,1 };
//char tmpbuf[128], timebuf[26], ampm[] = "AM";
time_t ltime;
struct _timeb tstruct;

DWORD start, finish, duration;

int i, j;
const int m = 200;
float e[2][m], u[2][m];
float Kp[2] = { 3.0, 5.0 }, Kd[2] = { 0.1, 0.1 };
float theta[2][m], dtheta[2][m], ddtheta[2][m], de[2][m], thetad[2][m], dthetad[2][m], ddthetad[2][m];
long lVal[2]; //Get USB Encoder value
char lpszDriverName1[100], lpszDriverName2[100];
float tempd = 0;
float tempdd = 0;

//char lpszDriverName1[100], lpszDriverName2[100];
//UINT resolution, encoding, resolution1, encoding1, channel[2] = { 0,1 };
HDEV hDevice[2];
char Bname[2][100];
using namespace std;

void EnumBrdProc(LPSTR lpszBrdName, LPSTR lpszDriverName, LPARAM lParam)
{
	// Make sure we can Init Board
	//MessageBox::Show("Hollo");
	(olDaInitialize((PTSTR)lpszBrdName, (LPHDEV)lParam));

	// Make sure Board has an A/D Subsystem 
	UINT uiCap = 0;
	olDaGetDevCaps(*((LPHDEV)lParam), OLDC_DAELEMENTS, &uiCap);

	//printf("Called%d\n",cnt);

}

void init(void) {
	olDaEnumBoards((DABRDPROC)EnumBrdProc, (LPARAM)&hDevice[0]);
	(olDaGetDeviceName(hDevice[0], (PTSTR)lpszDriverName1, 100));
	olDaGetDASS(hDevice[0], OLSS_DA, 0, &hDA);
	olDaSetDataFlow(hDA, OL_DF_SINGLEVALUE);
	olDaSetChannelListEntry(hDA, 0, channel[0]);
	olDaSetChannelListEntry(hDA, 0, channel[1]);
	olDaConfig(hDA);
	olDaGetResolution(hDA, &resolution);
	olDaGetEncoding(hDA, &encoding);
	olDaGetGainListEntry(hDA, 0, &gain);
	olDaGetRange(hDA, &max0, &min0);
	olDaVoltsToCode(min0, max0, gain, resolution, encoding, volt, &iValue);
	olDaPutSingleValue(hDA, iValue, channel[0], gain);
	olDaPutSingleValue(hDA, iValue, channel[1], gain);

}
void stopmotor()
{
	(olDaVoltsToCode(min0, max0, gain, resolution, encoding, 2.5, &iValue));  //To Stop Motor
	(olDaPutSingleValue(hDA, iValue, channel[0], gain));
	(olDaPutSingleValue(hDA, iValue, channel[1], gain));
}


void samplefun(double kp1, double kd1, double kp2, double kd2, double angle)
{
	// Position control 
	Kp[0] = kp1;
	Kp[1] = kp2;
	Kd[0] = kd1;
	Kd[1] = kd2;

	//MessageBox::Show("Given values are\n:KP1="+Kp[0].ToString()+"\nKp2="+Kp[1].ToString()+"\nKd1="+Kd[0].ToString()+"\nKd2="+Kd[1].ToString()+"\nDesired Angle="+angle.ToString());			 

	///	FILE *fp; //for Sample reading
	//	fopen_s(&fp, "D:\Sample.txt", "w");


	//CHECKERROR(olDaVoltsToCode(min,max,gain,resolution,encoding,volt,&iValue));  //To Stop Motor
	//CHECKERROR(olDaPutSingleValue(hDA,iValue,channel[0],gain));

	//CHECKERROR(olDaVoltsToCode(min,max,gain,resolution,encoding,volt,&iValue));  //To Stop Motor
	//CHECKERROR(olDaPutSingleValue(hDA,iValue,channel[1],gain));


	//CHECKERROR(olDaVoltsToCode(min,max,gain,resolution,encoding,volt,&iValue));  //To Stop Motor
	//CHECKERROR(olDaPutSingleValue(hDA1,iValue,channel[1],gain));

	//FILE *fp; //for Sample reading
	//fopen_s(&fp, "D:\Sample.txt", "w");
	NumberOfUSB1s = USB1Init();
	//printf("Number of USB1s = %d\n", NumberOfUSB1s);
	//MessageBox::Show("No of USB1 connected:"+NumberOfUSB1s.ToString());

	blnResult = USB1ReturnModuleAddress(0, &ModuleAddress);
	if (blnResult == false) printf("Cannot read Module Address!");
	else
	{
		(olDaVoltsToCode(min0, max0, gain, resolution, encoding, volt, &iValue));  //To Stop Motor
		(olDaPutSingleValue(hDA, iValue, channel[0], gain));
		(olDaPutSingleValue(hDA, iValue, channel[1], gain));
		//MessageBox::Show("Motor Stopped, Plz set the position of motor and press ok...");			

		//USB1SetIncMaxCount(ModuleAddress, 0, 64000); //Reset the USBEncoder
		//	USB1SetIncMaxCount(ModuleAddress, 1, 64000);

		// ************ Position Control Reference Trajectories *********
		for (i = 0; i < m; i++)
		{
			for (j = 0; j<2; j++)
			{
				thetad[j][i] = angle;
				dthetad[j][i] = 0.0;
				u[j][i] = 0.0;
				de[j][i] = 0.0;
			}
		}

		for (j = 0; j < 2; j++)
		{
			e[j][0] = angle;
			USB1GetIncPosition(ModuleAddress, j, &lVal[j]);
			theta[j][0] = 360.0*((float(lVal[j] % 64000)) / 64000.0);
			dtheta[j][0] = 0.0;
			//printf("theta=%f \t Encoder=%ld\n", theta[j][0], lVal[j]);
			//	fprintf(fp, "lVal=%f\t theta= %f\n", lVal[j], theta[j][0]);

		}
		//	_strtime_s(tmpbuf, 128);

		start = GetTickCount();


		//fprintf(fp, "Theta[1]\tdTheta[1]\tddtheta\tu[i]\n");

		for (i = 1; i < m; i++)
		{
			for (j = 0; j < 2; j++)
			{
				USB1GetIncPosition(ModuleAddress, j, &lVal[j]);
				theta[j][i] = 360.0*((float(lVal[j] % 64000)) / 64000.0);
				//	printf("theta=%f \t Encoder=%ld\n", theta[j][0], lVal[j]);
				//	dtheta[j][i] = (theta[j][i] - theta[j][i - 1]) / Ts;
				tempd = (theta[j][i] - theta[j][i - 1]) / Ts;
				dtheta[j][i] = filterloop(tempd);


				//ddtheta[j][i] = (dtheta[j][i] - dtheta[j][i - 1]) / Ts;
				tempdd = (dtheta[j][i] - dtheta[j][i - 1]) / Ts;
				ddtheta[j][i] = filter(tempdd);


				e[j][i] = thetad[j][i] - theta[j][i];


				u[j][i] = (Kp[j] * e[j][i]) / 100.0;
				//printf("theta=%f \t Encoder=%ld\t u[j][i]=%f\n", theta[j][0], lVal[j], u[j][i]);
				//printf("%f\n",u[j][i]);
				//	u[j][i] = -(Kp[j] * e[j][i] + Kd[j] * Derivative(e[j][i], e[j][i - 1])) / 100;
				if (u[j][i] > 2.5)
					u[j][i] = 2.5;
				else if (u[j][i] < -2.5)
					u[j][i] = -2.5;
				u[j][i] = u[j][i] + (float)2.5;
				//u[j][i] =2.4;


				(olDaVoltsToCode(min0, max0, gain, resolution, encoding, u[j][i], &iValue));
				(olDaPutSingleValue(hDA, iValue, channel[j], gain));
			}
			//	printf("theta [0][i]=%f\t    theta[1][i]= %f\n",theta[0][i],theta[1][i]);

			//fprintf(fp, "%f\t%f\t%f\t%f\n", theta[0][i], dtheta[0][i], ddtheta[0][i], u[0][i]);

		}

		//fprintf(fp, "%f\t%f\t%f\t%f\n", theta[0][i], dtheta[0][i], ddtheta[0][i], resolution);
		//	_strtime_s(tmpbuf, 128);
		//	fprintf(fp,"OS time:\t\t\t\t%s\n", tmpbuf);
		finish = GetTickCount();
		duration = finish - start;
		Ts = (float)duration / m;
		//fprintf(fp, "%f\t%d\n", duration, m);
		//fclose(fp);
	}

	//stopmotor();
}

/*OpenCv functions*/




string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();

}
void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void drawObject(vector<Fruit> theFruits, Mat &frame) {

	for (int i = 0; i<theFruits.size(); i++) {

		cv::circle(frame, cv::Point(theFruits.at(i).getXPos(), theFruits.at(i).getYPos()), 10, cv::Scalar(0, 0, 255));
		cv::putText(frame, intToString(theFruits.at(i).getXPos()) + " , " + intToString(theFruits.at(i).getYPos()), cv::Point(theFruits.at(i).getXPos(), theFruits.at(i).getYPos() + 20), 1, 1, Scalar(0, 255, 0));
		cv::putText(frame, theFruits.at(i).getType(), cv::Point(theFruits.at(i).getXPos(), theFruits.at(i).getYPos() - 30), 1, 2, theFruits.at(i).getColour());
	}
}



int getAngle(vector<Fruit> object) {
	int x1;
	int x2;
	int y1;
	int y2;
	x1 = object.at(0).getXPos();
	y1 = object.at(0).getYPos();
	x2 = object.at(1).getXPos();
	y2 = object.at(1).getYPos();
	int degree;
	degree = (180 / pi)*(atan((abs(y2 - y1)) / (0.000001 + abs(x2 - x1))));


	//degree = (180 / pi)*(atan((abs(&object.at(1).getYPos - &object.at(0).getYPos)) / (abs(&object.at(1).getXPos - &object.at(0).getXPos))));
	if (x1 < 220) {
		return 180 - degree;
	}

	else {
		return degree;
	}
	//return degree;


}
void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}
void trackFilteredObject(Mat threshold, Mat HSV, Mat &cameraFeed) {


	vector <Fruit> apples;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA) {

					Fruit apple;

					apple.setXPos(moment.m10 / area);
					apple.setYPos(moment.m01 / area);


					apples.push_back(apple);

					objectFound = true;

				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) {
				//draw object location on screen
				drawObject(apples, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}
void trackFilteredObject(Fruit theFruit, Mat threshold, Mat HSV, Mat &cameraFeed) {


	vector <Fruit> apples;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA) {

					Fruit apple;

					apple.setXPos(moment.m10 / area);
					apple.setYPos(moment.m01 / area);
					apple.setType(theFruit.getType());
					apple.setColour(theFruit.getColour());

					apples.push_back(apple);

					objectFound = true;

				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) {
				//draw object location on screen
				drawObject(apples, cameraFeed);
				deg = getAngle(apples);
			
				
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);

	}
}

/*the main function begins here*/

int main()

{

	/*motor control*/
	init();
	stopmotor();
	Sleep(500);
	USB1SetIncMaxCount(ModuleAddress, 0, 64000);



	/*opencv*/
	bool calibrationMode = false;



	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;

	if (calibrationMode) {
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	while(1){
	capture.read(cameraFeed);
	//convert frame from BGR to HSV colorspace
	cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);

	if (calibrationMode == true) {
		//if in calibration mode, we track objects based on the HSV slider values.
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
		morphOps(threshold);
		imshow(windowName2, threshold);
		trackFilteredObject(threshold, HSV, cameraFeed);
	}
	else {
		//create some temp fruit objects so that
		//we can use their member functions/information
		Fruit apple("Object"), banana("banana"), cherry("cherry");


		//first find apples
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		inRange(HSV, apple.getHSVmin(), apple.getHSVmax(), threshold);
		morphOps(threshold);
		trackFilteredObject(apple, threshold, HSV, cameraFeed);
		//then bananas
		/*		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		inRange(HSV, banana.getHSVmin(), banana.getHSVmax(), threshold);
		morphOps(threshold);
		trackFilteredObject(banana, threshold, HSV, cameraFeed);
		//then cherries
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		inRange(HSV, cherry.getHSVmin(), cherry.getHSVmax(), threshold);
		morphOps(threshold);
		tqrackFilteredObject(cherry, threshold, HSV, cameraFeed);*/





	}

	//show frames 
	//imshow(windowName2,threshold);

	imshow(windowName, cameraFeed);

	//int deg = getAngle(apples);
	//imshow(windowName1,HSV);
	//	printf("angle=%d\n",getAngle())
	//printf("degree=%d\n", getAngle(apples));
	//delay 30ms so that screen can refresh.
	//image will not appear without this waitKey() command
	printf("Angle=%d\n", deg);
	samplefun(5.0, 0, 2.0, 0, deg+10);
	if(waitKey(30)==99)break;
	
	//while (1) {
		
		

	}









    return 0;
}

