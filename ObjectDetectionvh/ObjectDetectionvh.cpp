// ObjectDetectionvh.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <Windows.h>
#include <iomanip>


using namespace cv;
using namespace std;


int *getCoordinates();
void filterFrame();
void mappingCurser(int currentPosX, int currentPosY);


int transformedPosition[4];
int readPosition[4][2];

int main(int argc, char** argv)
{
	int *getPosition;
	
	//get each slides edge position
	for (int k = 0; k < 4; k++) {
		getPosition = getCoordinates();
		for (int i = 0; i < 2; i++) {
			readPosition[k][i] = getPosition[i];
		}		
		cout << "Getting coordinates";
		waitKey(2000);		
	}
	
	cout << "Calibration Finished \n Coordinates points detected \n";

	for (int j = 0; j < 4; j++) {
		cout << "\n\ncoordinates of point " << j+1 << "\n";
		for (int m = 0; m < 2; m++) {
			if (m == 0) {
				cout << "X :" << readPosition[j][m] << "\n";
			}
			else if (m == 1) {
				cout << "Y :" << readPosition[j][m] << "\n";
			}			
		}
		cout << "\n";
	}

	int xn1 = (readPosition[0][0] + readPosition[1][0]) / 2;
	int xn2 = (readPosition[2][0] + readPosition[3][0]) / 2;

	int yn1 = (readPosition[0][1] + readPosition[3][1]) / 2;
	int yn2 = (readPosition[1][1] + readPosition[2][1]) / 2;

	/*

	yn1  *****
	yn2	 *****
		xn1	xn2
	
	*/
	transformedPosition[0] = xn1;
	transformedPosition[1] = xn2;
	transformedPosition[2] = yn1;
	transformedPosition[3] = yn2;

	filterFrame();


	cout << "main function end";
	return 0;

}


int *getCoordinates() {

	int position[2];
	bool detection = false;

	VideoCapture cap(1); //capture the video from web cam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam. Or connect a web cam" << endl;
		//return -1;
	}


	while (true)
	{
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}		

		imshow("Original", imgOriginal);
		Mat imgHSV;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		Mat imgThresholded;
		Mat imgThresholded1;
		//Mat imgThresholded1;
		

		inRange(imgHSV, Scalar(140, 114, 117), Scalar(179, 255, 255), imgThresholded); //Threshold the image
		
		//inRange(imgOriginal, Scalar(200, 130, 210), Scalar(235, 150, 245), imgThresholded);
		//inRange(imgOriginal, Scalar(230, 50, 230), Scalar(255, 70, 255), imgThresholded);

		/*Mat red_hue_image;
		addWeighted(imgThresholded, 1.0, imgThresholded, 1.0, 0.0, red_hue_image);
		GaussianBlur(red_hue_image, red_hue_image, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
		HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);*/


		//Mat imgThresholded2;
		//Get the blue color object BGR values
		//inRange(imgOriginal, Scalar(200, 0, 0), Scalar(255, 50, 50), imgThresholded2);

		//morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		
		imshow("Thresholded Image", imgThresholded);

		int x1 = 0; 
		int x2 = 0;

		x1 = countNonZero(imgThresholded);
		//x2 = countNonZero(imgThresholded2);

		if (x1 > x2) {
			Moments oMoments = moments(imgThresholded);

			double dM01 = oMoments.m01;
			double dM10 = oMoments.m10;
			double dArea = oMoments.m00;
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			//cout << "Pink color object detected.\n";
			
			if (dArea > 100000 ){
				position[0] = posX;
				position[1] = posY;

				cout << "Coordinates : X = " << posX << "and Y = " << posY << "\n";
				detection = true;
			}
			

			//Point pnt = imgThresholded.at<Point>;

			/*Vec3b colour = drawing.at<Vec3b>(Point(x, y));
			if (colour.val[0] == 255 && colour.val[1] == 255 && colour.val[2] == 255)*/
		}
		/*else if (x2 >  x1) {
			Moments oMoments = moments(imgThresholded2);


			double dM01 = oMoments.m01;
			double dM10 = oMoments.m10;
			double dArea = oMoments.m00;
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			cout << "Blue color object detected.\n";
			cout << "Coordinates : X = " << posX << "and Y = " << posY;
		}*/
		else {
			cout << "Nothing Detected\n";
		}



		//Mat imgHSV;
		//cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		//inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		//imshow("Thresholded Image", imgThresholded); //show the thresholded image
		//imshow("Original", imgOriginal); //show the original image

		//if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		//{
		//	cout << "esc key is pressed by user" << endl;
		//	break;
		//}

		if (detection == true) {
			break;
		}


	}// end of while loop

	return position;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////



//Filter area from the video frame

void filterFrame() {

	int position[2];


	VideoCapture cap(1); //capture the video from web cam
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam. Or connect a web cam" << endl;
		//return -1;
	}

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(140, 114, 117), Scalar(179, 255, 255), imgThresholded);  //Threshold the image

																									  //morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image

		int x1 = 0;
		int x2 = 0;

		x1 = countNonZero(imgThresholded);
		//x2 = countNonZero(imgThresholded2);

		if (x1 > x2) {
			Moments oMoments = moments(imgThresholded);

			double dM01 = oMoments.m01;
			double dM10 = oMoments.m10;
			double dArea = oMoments.m00;
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			//cout << "Pink color object detected.\n";

			if (dArea > 100000) {

				position[0] = posX;
				position[1] = posY;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//Here goes the function to get the Mapped positions with the presentation slides
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				




				INPUT ip;
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = 0; // hardware scan code for key
				ip.ki.time = 0;
				ip.ki.dwExtraInfo = 0;

				INPUT Input = { 0 };  //For mouse input

				if ((posX >= transformedPosition[0] && posX <= transformedPosition[1]) && (posY <= transformedPosition[2] && posY >= transformedPosition[3])) {
					cout << "Inside the filter" << "\n";
					//If inside the filter start writing

					
					// Press the "A" key
					ip.ki.wVk = 0x11; // virtual-key code for the "ctrl" key
					ip.ki.dwFlags = 0; // 0 for key press
					SendInput(1, &ip, sizeof(INPUT));

					ip.ki.wVk = 0x50; // virtual-key code for the "ctrl" key
					ip.ki.dwFlags = 0; // 0 for key press
					SendInput(1, &ip, sizeof(INPUT));

					ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
					SendInput(1, &ip, sizeof(INPUT));
		
					// left down 
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					SendInput(1, &Input, sizeof(INPUT));	

					SetCursorPos(posX, posY);
				}
				else {
					cout << "Not inside" << "\n";
				}
				
				//Mouse release - left up
				ZeroMemory(&Input, sizeof(INPUT));
				Input.type = INPUT_MOUSE;
				Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(1, &Input, sizeof(INPUT));

				//Press Esc to quit from pen
				ip.ki.wVk = 0x1B; // virtual-key code for the "Esc" key
				ip.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &ip, sizeof(INPUT));

				ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &ip, sizeof(INPUT));				
			}



			if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			{
				cout << "esc key is pressed by user" << endl;
				break;
			}
		}


	}
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void mappingCurser(int currentPosX, int currentPosY) {
	int posX1, posX2, posY1, posY2;

	posX1 = transformedPosition[0];
	posX2 = transformedPosition[1];
	posY1 = transformedPosition[2];
	posY2 = transformedPosition[3];

	//Default resolution of the screen  1366x768





}