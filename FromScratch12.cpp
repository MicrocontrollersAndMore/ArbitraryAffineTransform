// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

// function prototypes ////////////////////////////////////////////////////////////////////////////
cv::Mat getFlipMatrix(cv::Mat imgInput, cv::Point2f p2fCenter, bool flipOnXAxis, bool flipOnYAxis);
cv::Mat myFlip(cv::Mat flipMatrix, cv::Mat imgInput);
cv::Mat getRotateAndScaleMatrix(cv::Point2f p2fCenter, double dblAngleInDegrees, double dblScale);
cv::Mat rotateAndScale(cv::Mat transformationMatrix, cv::Mat imgInput);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	cv::Mat imgOriginal;				// input image
	cv::Mat imgFlipped;
	cv::Mat imgFlippedRotatedAndScaled;

	imgOriginal = cv::imread("lena.pgm", CV_LOAD_IMAGE_GRAYSCALE);			// open image

	if (imgOriginal.empty()) {									// if unable to open image
		std::cout << "error: image not read from file\n\n";		// show error message on command line
		return(0);												// and exit program
	}

	bool flipOnXAxis = true;
	bool flipOnYAxis = false;
	double dblDegreesOfRotation = 47.0;
	double dblScale = 1.5;
	
	if (flipOnXAxis || flipOnYAxis) {
		cv::Mat flipMatrix = getFlipMatrix(imgOriginal, cv::Point(imgOriginal.cols / 2, imgOriginal.rows / 2), flipOnXAxis, flipOnYAxis);
		std::cout << "flipMatrix = " << std::endl << flipMatrix << std::endl << std::endl << std::endl;
		imgFlipped = myFlip(flipMatrix, imgOriginal);
	} else {
		imgFlipped = imgOriginal.clone();
	}

	cv::Mat rotateAndScaleMatrix = getRotateAndScaleMatrix(cv::Point(imgOriginal.cols / 2, imgOriginal.rows / 2), dblDegreesOfRotation, dblScale);

	std::cout << "rotateAndScaleMatrix = " << std::endl << rotateAndScaleMatrix << std::endl << std::endl << std::endl;

	imgFlippedRotatedAndScaled = rotateAndScale(rotateAndScaleMatrix, imgFlipped);
	
	cv::imshow("imgOriginal", imgOriginal);		// show windows
	cv::imshow("imgFlipped", imgFlipped);
	cv::imshow("imgFlippedRotatedAndScaled", imgFlippedRotatedAndScaled);

	cv::imwrite("imgFlippedRotatedAndScaled.png", imgFlippedRotatedAndScaled);

	cv::waitKey(0);					// hold windows open until user presses a key

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat getFlipMatrix(cv::Mat imgInput, cv::Point2f p2fCenter, bool flipOnXAxis, bool flipOnYAxis) {
	cv::Mat flipMatrix(2, 3, CV_64F);
	
	double dblXOffset = 0.0;
	double dblYOffset = 0.0;
	
	if (!flipOnXAxis) {
		flipMatrix.at<double>(0, 0) = 1;
	} else if (flipOnXAxis) {
		flipMatrix.at<double>(0, 0) = -1;
		dblXOffset = dblXOffset + imgInput.cols - 1;
	}

	flipMatrix.at<double>(0, 1) = 0.0;
	flipMatrix.at<double>(1, 0) = 0.0;

	if (!flipOnYAxis) {
		flipMatrix.at<double>(1, 1) = 1;
	}
	else if (flipOnYAxis) {
		flipMatrix.at<double>(1, 1) = -1;
		dblYOffset = dblYOffset + imgInput.rows - 1;
	}

	flipMatrix.at<double>(0, 2) = dblXOffset;			// x offset
	flipMatrix.at<double>(1, 2) = dblYOffset;			// y offset
	
	return(flipMatrix);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat myFlip(cv::Mat flipMatrix, cv::Mat imgInput) {
	cv::Mat imgFlipped(imgInput.rows, imgInput.cols, CV_8UC1);		// this will be the return value

	for (int x = 0; x < imgInput.cols; x++) {
		for (int y = 0; y < imgInput.rows; y++) {
			int xPrime = (int)((flipMatrix.at<double>(0, 0) * x) + (flipMatrix.at<double>(0, 1) * y) + flipMatrix.at<double>(0, 2));
			int yPrime = (int)((flipMatrix.at<double>(1, 0) * x) + (flipMatrix.at<double>(1, 1) * y) + flipMatrix.at<double>(1, 2));

			if (xPrime >= 0 && xPrime < imgFlipped.cols && yPrime >= 0 && yPrime < imgFlipped.rows) {
				imgFlipped.at<uchar>(yPrime, xPrime) = imgInput.at<uchar>(y, x);
			}

		}
	}

	return(imgFlipped);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat getRotateAndScaleMatrix(cv::Point2f p2fCenter, double dblAngleInDegrees, double dblScale) {
	cv::Mat rotateAndScaleMatrix(2, 3, CV_64F);								// this will be the return value
	
	double dblAngleInRadians = dblAngleInDegrees * (3.14159 / 180);

	double dblXOffset = ((1 - (cos(dblAngleInRadians) * dblScale)) * p2fCenter.x) - ((sin(dblAngleInRadians) * dblScale) * p2fCenter.y);
	double dblYOffset = ((sin(dblAngleInRadians) * dblScale) * p2fCenter.x) + ((1 - (cos(dblAngleInRadians) * dblScale)) * p2fCenter.y);
	
	rotateAndScaleMatrix.at<double>(0, 0) = cos(dblAngleInRadians) * dblScale;
	rotateAndScaleMatrix.at<double>(0, 1) = sin(dblAngleInRadians) * dblScale;
	rotateAndScaleMatrix.at<double>(0, 2) = dblXOffset;
	rotateAndScaleMatrix.at<double>(1, 0) = -sin(dblAngleInRadians) * dblScale;
	rotateAndScaleMatrix.at<double>(1, 1) = cos(dblAngleInRadians) * dblScale;
	rotateAndScaleMatrix.at<double>(1, 2) = dblYOffset;
	
	return(rotateAndScaleMatrix);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat rotateAndScale(cv::Mat transformationMatrix, cv::Mat imgInput) {
	cv::Mat imgRotatedAndScaled(imgInput.rows, imgInput.cols, CV_8UC1, cv::Scalar(0.0));

	cv::Mat matPixelWasSet(imgInput.rows, imgInput.cols, CV_8UC1, cv::Scalar(0.0));

	for (int x = 0; x < imgInput.cols; x++) {
		for (int y = 0; y < imgInput.rows; y++) {
			int xPrime = (int)((transformationMatrix.at<double>(0, 0) * x) + (transformationMatrix.at<double>(0, 1) * y) + transformationMatrix.at<double>(0, 2));
			int yPrime = (int)((transformationMatrix.at<double>(1, 0) * x) + (transformationMatrix.at<double>(1, 1) * y) + transformationMatrix.at<double>(1, 2));

			if (xPrime >= 0 && xPrime < imgRotatedAndScaled.cols && yPrime >= 0 && yPrime < imgRotatedAndScaled.rows) {
				imgRotatedAndScaled.at<uchar>(yPrime, xPrime) = imgInput.at<uchar>(y, x);
				matPixelWasSet.at<uchar>(yPrime, xPrime) = 1;
			}
			
		}
	}

	int myCount = 0;
	
	for (int x = 0; x < imgRotatedAndScaled.cols; x++) {
		for (int y = 0; y < imgRotatedAndScaled.rows; y++) {
			if (matPixelWasSet.at<uchar>(y, x) == 0) {

				int intTotalOfPixelsAround = 0;
				int intCountOfPixelsAround = 0;
				/*
				int intWindowSize = 3;

				for (int innerX = 0; innerX < intWindowSize; innerX++) {
				for (int innerY = 0; innerY < intWindowSize; innerY++) {
				int intOffset = intWindowSize / 2;

				if ((x + innerX - intOffset) >= 0 && (x + innerX - intOffset + 2) <= imgRotatedAndScaled.cols &&
					(y + innerY - intOffset) >= 0 && (y + innerY - intOffset + 2) <= imgRotatedAndScaled.rows) {
					if (imgRotatedAndScaled.at<uchar>(y + innerY - intOffset, x + innerX - intOffset) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y + innerY - intOffset, x + innerX - intOffset);
						intCountOfPixelsAround++;
					}
				}
				*/

				/*
				if ((x - 1) >= 0 && (y - 1) >= 0) {													// left 1, up 1
					if (imgRotatedAndScaled.at<uchar>(y - 1, x - 1) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y - 1, x - 1);
						intCountOfPixelsAround++;
					}
				}
				*/
				if ((y - 1) >= 0) {																	// up 1
					if (imgRotatedAndScaled.at<uchar>(y - 1, x) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y - 1, x);
						intCountOfPixelsAround++;
					}
				}
				/*
				if ((x + 1) < imgRotatedAndScaled.cols && (y - 1) >= 0) {							// right 1, up 1
					if (imgRotatedAndScaled.at<uchar>(y - 1, x + 1) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y - 1, x + 1);
						intCountOfPixelsAround++;
					}
				}
				*/
				if ((x + 1) < imgRotatedAndScaled.cols) {											// right 1
					if (imgRotatedAndScaled.at<uchar>(y, x + 1) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y, x + 1);
						intCountOfPixelsAround++;
					}
				}
				/*
				if ((x + 1) < imgRotatedAndScaled.cols && (y + 1) < imgRotatedAndScaled.rows) {		// right 1, down 1
					if (imgRotatedAndScaled.at<uchar>(y + 1, x + 1) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y + 1, x + 1);
						intCountOfPixelsAround++;
					}
				}
				*/
				if ((y + 1) < imgRotatedAndScaled.rows) {											// down 1
					if (imgRotatedAndScaled.at<uchar>(y + 1, x) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y + 1, x);
						intCountOfPixelsAround++;
					}
				}
				/*
				if ((x - 1) >= 0 && (y + 1) < imgRotatedAndScaled.rows) {							// left 1, down 1
					if (imgRotatedAndScaled.at<uchar>(y + 1, x - 1) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y + 1, x - 1);
						intCountOfPixelsAround++;
					}
				}
				*/
				if ((x - 1) >= 0) {																	// left 1
					if (imgRotatedAndScaled.at<uchar>(y, x - 1) > 0) {
						intTotalOfPixelsAround = intTotalOfPixelsAround + imgRotatedAndScaled.at<uchar>(y, x - 1);
						intCountOfPixelsAround++;
					}
				}

				imgRotatedAndScaled.at<uchar>(y, x) = intTotalOfPixelsAround / intCountOfPixelsAround;
			}

		}

	}
	return(imgRotatedAndScaled);
}





