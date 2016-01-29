#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<stdio.h>
#include<stdlib.h>

//--------- Potrzebne do komunikacji - serial port ----------------------//
#include <Windows.h>

using namespace std;
using namespace cv;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	VideoCapture capKamera(0);													// declare a VideoCapture object and associate to webcam, 0 => use 1st webcam
	if (capKamera.isOpened() == false) {											
		cout << "error: capWebcam not accessed successfully\n\n";				
		return(0);																
	}

////---------------Okno z suwakami ------------------------///
	namedWindow("Ustawienia koloru", CV_WINDOW_AUTOSIZE);
												// dla czerwonego:		//dla niebieskiego
	int iLowH = 146;								// 146					//53
	int iHighH = 177;							// 177					//120

	int iLowS = 146;							//146					//102
	int iHighS = 233;							//233					//255

	int iLowV = 98;								//98					//60
	int iHighV = 255;							//255					//255

	static int posX = 0;
	static int posY = 0;
	
	cvCreateTrackbar("LowH", "Ustawienia koloru", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Ustawienia koloru", &iHighH, 179);

	cvCreateTrackbar("LowS", "Ustawienia koloru", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Ustawienia koloru", &iHighS, 255);

	cvCreateTrackbar("LowV", "Ustawienia koloru", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Ustawienia koloru", &iHighV, 255);


//----------------- Setup serial port connection and needed variables. -------------------------------//

	HANDLE hSerial = CreateFile("COM3", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hSerial != INVALID_HANDLE_VALUE)
	{
		printf("Port opened! \n");

		DCB dcbSerialParams;
		GetCommState(hSerial, &dcbSerialParams);

		dcbSerialParams.BaudRate = CBR_9600;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.Parity = NOPARITY;
		dcbSerialParams.StopBits = ONESTOPBIT;

		SetCommState(hSerial, &dcbSerialParams);
	}
	else
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			printf("Serial port doesn't exist! \n");
		}

		printf("Error while setting up serial port! \n");
	}

	char outputChars[] = "n";
	DWORD btsIO;


	Mat imgOriginal;																// input image
	Mat imgHSV;
	//Mat imgThreshLow;																// kolor ustawiany na sztywno
	//Mat imgThreshHigh;
	Mat imgThresh;

	vector<Vec3f> v3fCircles;														// 3 element vector of floats, this will be the pass by reference output of HoughCircles()

	char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && capKamera.isOpened()) {						// until the Esc key is pressed or webcam connection is lost
		bool blnFrameReadSuccessfully = capKamera.read(imgOriginal);				// get next frame

		if (!blnFrameReadSuccessfully || imgOriginal.empty()) {						// if frame not read successfully
			cout << "error: frame not read from webcam\n";							// print error message to std out
			break;																	// and jump out of while loop
		}

		cvtColor(imgOriginal, imgHSV, CV_BGR2HSV);

		//-------------------------- kolor czerwony --------------------------------//

		//inRange(imgHSV, Scalar(0, 155, 155), Scalar(20, 255, 255), imgThreshLow);
		//inRange(imgHSV, Scalar(165, 155, 155), Scalar(179, 255, 255), imgThreshHigh);
		//add(imgThreshLow, imgThreshHigh, imgThresh);

		//---------------------------------kolor z paskow -------------------------------------//
	
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresh);

		//------------------------------------------------------------------------------------//

		GaussianBlur(imgThresh, imgThresh, Size(3, 3), 0);

		
		Mat structuringElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		// usuwanie malych obiektow z tla 
		erode(imgThresh, imgThresh, structuringElement);
		dilate(imgThresh, imgThresh, structuringElement);

		// wypelnienie tla
		dilate(imgThresh, imgThresh, structuringElement);
		erode(imgThresh, imgThresh, structuringElement);

		// fill circles vector with all circles in processed image
		HoughCircles(imgThresh,					// input image
			v3fCircles,							// function output (must be a standard template library vector
			CV_HOUGH_GRADIENT,					// two-pass algorithm for detecting circles, this is the only choice available
			2,									// size of image / this value = "accumulator resolution", i.e. accum res = size of image / 2
			imgThresh.rows / 4,					// min distance in pixels between the centers of the detected circles
			100,								// high threshold of Canny edge detector (called by cvHoughCircles)						
			50,									// low threshold of Canny edge detector (set at 1/2 previous value)
			10,									// min circle radius (any circles with smaller radius will not be returned)
			400);								// max circle radius (any circles with larger radius will not be returned)

		

		for (int i = 0; i < v3fCircles.size(); i++) {							// for each circle . . . - usuniêto [i] - jest tylko dla 1 obiektu
																				// show ball position x, y, and radius to command line
			cout << "Pozycja obiektu x = " 
				<< v3fCircles[0][0]												// x position of center point of circle
				<< ", y = " << v3fCircles[0][1]									// y position of center point of circle
				<< ", promien = " << v3fCircles[0][2] << "\n";					// radius of circle

		// draw small green circle at center of detected object
			circle(imgOriginal,													
				Point((int)v3fCircles[0][0], (int)v3fCircles[0][1]),			
				1,																
				Scalar(0, 255, 0),												
				CV_FILLED);														

		// draw red circle around the detected object
			circle(imgOriginal,													
				Point((int)v3fCircles[0][0], (int)v3fCircles[0][1]),			
				(int)v3fCircles[0][2],											
				Scalar(0, 0, 255),												
				1);																
				

			Size s = imgOriginal.size();
			int medx = s.width / 2;

			posX = v3fCircles[0][0];
			int policz_l, policz_r;
			
			if (posX < (medx - medx / 3)) { outputChars[0] = 'l'; policz_l += 1; policz_r = 0; }
			else if (posX >(medx + medx / 3)) { outputChars[0] = 'r';  policz_r += 1; policz_l = 0;}
			else { outputChars[0] = 'n';  policz_l = 0; policz_r = 0;}

			if (policz_l == 3 || policz_r == 3 || outputChars[0] == 'n') 
			{
				WriteFile(hSerial, outputChars, strlen(outputChars), &btsIO, NULL);
				policz_l = 0; policz_r = 0;
			}

			cout << "jedzie w " << outputChars[0] << "\n";
		
		}	// end for


		// declare windows
		namedWindow("Kamera", CV_WINDOW_AUTOSIZE);								
		namedWindow("Przetworzony", CV_WINDOW_AUTOSIZE);						
																				
		// show windows
		imshow("Kamera", imgOriginal);			
		imshow("Przetworzony", imgThresh);

		charCheckForEscKey = waitKey(1);									
	}	// end while

	return(0);
}