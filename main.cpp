#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

//--------- Potrzebne do komunikacji - serial port ----------------------//
#include <Windows.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	cv::VideoCapture capKamera(0);													// declare a VideoCapture object and associate to webcam, 0 => use 1st webcam

	if (capKamera.isOpened() == false) {											// check if VideoCapture object was associated to webcam successfully
		std::cout << "error: capWebcam not accessed successfully\n\n";				// if not, print error message to std out
		return(0);																	// and exit program
	}

////---------------Okno z suwakami ------------------------///
	cv::namedWindow("Ustawienia koloru", CV_WINDOW_AUTOSIZE);

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	static int posX = 0;
	static int posY = 0;
	
	cvCreateTrackbar("LowH", "Ustawienia koloru", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Ustawienia koloru", &iHighH, 179);

	cvCreateTrackbar("LowS", "Ustawienia koloru", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Ustawienia koloru", &iHighS, 255);

	cvCreateTrackbar("LowV", "Ustawienia koloru", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Ustawienia koloru", &iHighV, 255);

////----------------------------------------------------------------///
	
	cv::Mat imgOriginal;															// input image
	cv::Mat imgHSV;
	//cv::Mat imgThreshLow;															// kolor ustawiany na sztywno
	//cv::Mat imgThreshHigh;
	cv::Mat imgThresh;

	std::vector<cv::Vec3f> v3fCircles;												// 3 element vector of floats, this will be the pass by reference output of HoughCircles()

	char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && capKamera.isOpened()) {						// until the Esc key is pressed or webcam connection is lost
		bool blnFrameReadSuccessfully = capKamera.read(imgOriginal);				// get next frame

		if (!blnFrameReadSuccessfully || imgOriginal.empty()) {						// if frame not read successfully
			std::cout << "error: frame not read from webcam\n";						// print error message to std out
			break;																	// and jump out of while loop
		}

		cv::cvtColor(imgOriginal, imgHSV, CV_BGR2HSV);

		//----------------- kolor czerwony -----------------------//

		//cv::inRange(imgHSV, cv::Scalar(0, 155, 155), cv::Scalar(20, 255, 255), imgThreshLow);
		//cv::inRange(imgHSV, cv::Scalar(165, 155, 155), cv::Scalar(179, 255, 255), imgThreshHigh);
		//cv::add(imgThreshLow, imgThreshHigh, imgThresh);

		//------------------kolor z paskow -----------------------//
	
		inRange(imgHSV, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), imgThresh);

		//--------------------------------------------------------//

		cv::GaussianBlur(imgThresh, imgThresh, cv::Size(5, 5), 0);

		
		cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));

		// usuwanie malych obiektow z tla 
		cv::erode(imgThresh, imgThresh, structuringElement);
		cv::dilate(imgThresh, imgThresh, structuringElement);

		// wypelnienie tla
		cv::dilate(imgThresh, imgThresh, structuringElement);
		cv::erode(imgThresh, imgThresh, structuringElement);

		// fill circles vector with all circles in processed image
		cv::HoughCircles(imgThresh,				// input image
			v3fCircles,							// function output (must be a standard template library vector
			CV_HOUGH_GRADIENT,					// two-pass algorithm for detecting circles, this is the only choice available
			2,									// size of image / this value = "accumulator resolution", i.e. accum res = size of image / 2
			imgThresh.rows / 4,					// min distance in pixels between the centers of the detected circles
			100,								// high threshold of Canny edge detector (called by cvHoughCircles)						
			50,									// low threshold of Canny edge detector (set at 1/2 previous value)
			8,									// min circle radius (any circles with smaller radius will not be returned)
			400);								// max circle radius (any circles with larger radius will not be returned)

		

		for (int i = 0; i < v3fCircles.size(); i++) {							// for each circle . . . - usuni�to [i] - jest tylko dla 1 obiektu
																				// show ball position x, y, and radius to command line
			std::cout << "Pozycja obiektu x = " 
				<< v3fCircles[0][0]												// x position of center point of circle
				<< ", y = " << v3fCircles[0][1]									// y position of center point of circle
				<< ", promien = " << v3fCircles[0][2] << "\n";					// radius of circle

																				// draw small green circle at center of detected object
			cv::circle(imgOriginal,												// draw on original image
				cv::Point((int)v3fCircles[0][0], (int)v3fCircles[0][1]),		// center point of circle
				3,																// radius of circle in pixels
				cv::Scalar(0, 255, 0),											// draw pure green (remember, its BGR, not RGB)
				CV_FILLED);														// thickness, fill in the circle

																				// draw red circle around the detected object
			cv::circle(imgOriginal,												// draw on original image
				cv::Point((int)v3fCircles[0][0], (int)v3fCircles[0][1]),		// center point of circle
				(int)v3fCircles[0][2],											// radius of circle in pixels
				cv::Scalar(0, 0, 255),											// draw pure red (remember, its BGR, not RGB)
				1);																// thickness of circle in pixels
		}	// end for

		// declare windows
		cv::namedWindow("Obraz pierwotny", CV_WINDOW_AUTOSIZE);					// note: you can use CV_WINDOW_NORMAL which allows resizing the window
		cv::namedWindow("Obraz przetworzony", CV_WINDOW_AUTOSIZE);				// or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
																				// CV_WINDOW_AUTOSIZE is the default
		// show windows
		cv::imshow("Obraz pierwotny", imgOriginal);			
		cv::imshow("Obraz przetworzony", imgThresh);

		charCheckForEscKey = cv::waitKey(1);									// delay (in ms) and get key press, if any
	}	// end while

	return(0);
}