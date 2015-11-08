// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

// function prototypes ////////////////////////////////////////////////////////////////////////////
cv::Mat myAffineTransform(cv::Mat imgInput, cv::Point2f p2fCenterOfRotation, bool blnFlipHorizontal, bool blnFlipVertical, double dblAngleInDegrees, double dblScale);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	bool blnFlipHorizontal = true;						// affine transform options
	bool blnFlipVertical = false;
	double dblDegreesOfRotation = 17.0;
	double dblScale = 0.6;

	cv::Mat imgOriginal;							// declare various image variables
	cv::Mat imgGrayscale;
	cv::Mat imgTransformed;

	imgOriginal = cv::imread("SauberBMWF1.png");				// open image

	if (imgOriginal.empty()) {									// if unable to open image
		std::cout << "error: image not read from file\n\n";		// show error message on command line
		return(0);												// and exit program
	}

	if (imgOriginal.channels() == 3) {										// if image is color, convert to grayscale
		cv::cvtColor(imgOriginal, imgGrayscale, CV_BGR2GRAY);				// convert to grayscale
	}
	else if (imgOriginal.channels() == 1) {								// else if image is grayscale
		imgGrayscale = imgOriginal.clone();									// clone into grayscale
	}
	else {																// if not 1 channel or 3 channel
		std::cout << "error: image was not read as 1 or 3 channels\n\n";	// something went terribly wrong
		return(0);															// exit program
	}

	int64 begTickCount = cv::getTickCount();				// begin timing
	// call affine transform function
	imgTransformed = myAffineTransform(imgGrayscale, cv::Point(imgGrayscale.cols / 2, imgGrayscale.rows / 2), blnFlipHorizontal, blnFlipVertical, dblDegreesOfRotation, dblScale);

	int64 endTickCount = cv::getTickCount();				// end timing
	// calculate and display time
	std::cout << "image processing took " << (endTickCount - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	cv::imshow("imgGrayscale", imgGrayscale);				// show windows
	cv::imshow("imgTransformed", imgTransformed);

	cv::imwrite("imgTransformed.png", imgTransformed);		// write output image to file

	cv::waitKey(0);					// hold windows open until user presses a key

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat myAffineTransform(cv::Mat imgInput, cv::Point2f p2fCenterOfRotation, bool blnFlipHorizontal, bool blnFlipVertical, double dblAngleInDegrees, double dblScale) {

	int64 begTickCount = cv::getTickCount();

	cv::Mat imgTransformed(imgInput.rows, imgInput.cols, CV_8UC1, cv::Scalar(0.0));			// this will be the return value

	cv::Mat transformationMatrix(2, 2, CV_64F);								// declare transformation matrix

	double dblAngleInRadians = dblAngleInDegrees * (3.14159 / 180);			// convert angle of rotation from degrees to radians

	transformationMatrix.at<double>(0, 0) = cos(dblAngleInRadians) * dblScale;			// initial values for top row of transform matrix
	transformationMatrix.at<double>(0, 1) = -sin(dblAngleInRadians) * dblScale;

	transformationMatrix.at<double>(1, 0) = sin(dblAngleInRadians) * dblScale;			// initial values for bottom row of transform matrix
	transformationMatrix.at<double>(1, 1) = cos(dblAngleInRadians) * dblScale;

	// calculate offset for rotation and scaling, this is the tricky part !!
	// note that we can't calculate offset for x' y' until we are looping through the pixels
	double alpha = cos(dblAngleInRadians) * dblScale;
	double beta = -sin(dblAngleInRadians) * dblScale;

	double dblXOffset = ((1 - alpha) * p2fCenterOfRotation.x) - (beta * p2fCenterOfRotation.y);
	double dblYOffset = (beta * p2fCenterOfRotation.x) + ((1 - alpha) * p2fCenterOfRotation.y);

	// this data structure keeps track of which pixels in the final image
	cv::Mat matPixelWasSet(imgInput.rows, imgInput.cols, CV_8UC1, cv::Scalar(0.0));		// have been set versus which have not, this is necessary b/c
	// any pixels in the final image that have not been set will have
	// to be interpolated later

	std::cout << "T1 = " << (cv::getTickCount() - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	for (int x = 0; x < imgInput.cols; x++) {				// loop through original image
		for (int y = 0; y < imgInput.rows; y++) {
			// for each pixel in the original image, apply matrix multiplication to determine where that pixel will be in the output image
			int xPrime = (int)((transformationMatrix.at<double>(0, 0) * x) + (transformationMatrix.at<double>(0, 1) * y) + dblXOffset);
			int yPrime = (int)((transformationMatrix.at<double>(1, 0) * x) + (transformationMatrix.at<double>(1, 1) * y) + dblYOffset);

			if (blnFlipHorizontal) {										// if applicable, flip x' horizontally
				xPrime = xPrime + ((p2fCenterOfRotation.x - xPrime) * 2);
			}

			if (blnFlipVertical) {											// if applicable, flip y' vertically
				yPrime = yPrime + ((p2fCenterOfRotation.y - yPrime) * 2);
			}

			if (xPrime >= 0 && xPrime < imgTransformed.cols && yPrime >= 0 && yPrime < imgTransformed.rows) {	// if the x' y' value is within the bounds of the output image
				imgTransformed.at<uchar>(yPrime, xPrime) = imgInput.at<uchar>(y, x);							// set that pixel of the output image
				matPixelWasSet.at<uchar>(yPrime, xPrime) = 1;													// and mark that pixel has having been set in the data structure that keeps track of which pixels in the output image have been set
			}
		}
	}

	std::cout << "T2 = " << (cv::getTickCount() - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	// next we have to fill in any pixels via interpolation in the output image that have not been set
	for (int x = 0; x < imgTransformed.cols; x++) {			// loop through output image
		for (int y = 0; y < imgTransformed.rows; y++) {
			if (matPixelWasSet.at<uchar>(y, x) == 0) {		// if the current pixel has not been set

				// check if any of the 8 pixels down and to the right have been set, if so, we are not off the edge of the image
				// and so we should interpolate the current pixel
				if ((((x + 1) < imgTransformed.cols) && (imgTransformed.at<uchar>(y, x + 1) > 0)) ||
					(((y + 1) < imgTransformed.rows) && (imgTransformed.at<uchar>(y + 1, x) > 0)) ||
					(((x + 1) < imgTransformed.cols) && ((y + 1) < imgTransformed.rows) && (imgTransformed.at<uchar>(y + 1, x + 1) > 0)) ||
					(((x + 2) < imgTransformed.cols) && (imgTransformed.at<uchar>(y, x + 2) > 0)) ||
					(((y + 2) < imgTransformed.rows) && (imgTransformed.at<uchar>(y + 2, x) > 0)) ||
					(((x + 1) < imgTransformed.cols) && ((y + 2) < imgTransformed.rows) && (imgTransformed.at<uchar>(y + 2, x + 1) > 0)) ||
					(((x + 2) < imgTransformed.cols) && ((y + 1) < imgTransformed.rows) && (imgTransformed.at<uchar>(y + 1, x + 2) > 0)) ||
					(((x + 2) < imgTransformed.cols) && ((y + 2) < imgTransformed.rows) && (imgTransformed.at<uchar>(y + 2, x + 2) > 0))) {

					// strategy is to look at each of the 4 pixels up, down, left, and right of the current pixel,
					// make sure they are not out of bounds of the image and make sure they have been set (non-zero),
					// if so, add the value

					int intTotalOfPixelsAround = 0;			// total value of as many as 4 neighboring pixles
					int intCountOfPixelsAround = 0;			// # of neighboring pixels we are considering, can be as many as 4

					// up 1
					if ((y - 1) >= 0) {																	// if neighboring pixel is within bounds of image
						if (imgTransformed.at<uchar>(y - 1, x) > 0) {									// and if neighboring pixel is non-zero
							intTotalOfPixelsAround = intTotalOfPixelsAround + imgTransformed.at<uchar>(y - 1, x);	// add neighboring pixel value to sum total
							intCountOfPixelsAround++;																// increment count of valid neighboring pixels
						}
					}

					if ((x + 1) < imgTransformed.cols) {					// right 1
						if (imgTransformed.at<uchar>(y, x + 1) > 0) {
							intTotalOfPixelsAround = intTotalOfPixelsAround + imgTransformed.at<uchar>(y, x + 1);
							intCountOfPixelsAround++;
						}
					}

					if ((y + 1) < imgTransformed.rows) {					// down 1
						if (imgTransformed.at<uchar>(y + 1, x) > 0) {
							intTotalOfPixelsAround = intTotalOfPixelsAround + imgTransformed.at<uchar>(y + 1, x);
							intCountOfPixelsAround++;
						}
					}

					if ((x - 1) >= 0) {										// left 1
						if (imgTransformed.at<uchar>(y, x - 1) > 0) {
							intTotalOfPixelsAround = intTotalOfPixelsAround + imgTransformed.at<uchar>(y, x - 1);
							intCountOfPixelsAround++;
						}
					}

					if (intCountOfPixelsAround > 0) {														// if at least 1 valid neighboring pixel was found
						imgTransformed.at<uchar>(y, x) = intTotalOfPixelsAround / intCountOfPixelsAround;	// assign average of valid neighboring pixels to current pixel of output image
					}
				}
			}

		}

	}
	std::cout << "T3 = " << (cv::getTickCount() - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	return(imgTransformed);
}


