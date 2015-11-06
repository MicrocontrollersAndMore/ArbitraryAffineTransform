// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

#define FLIP_HORIZONTAL	1
#define FLIP_VERTICAL	0
#define FLIP_HORIZONTAL_AND_VERTICAL -1

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	bool blnFlipHorizontal = true;						// affine transform options
	bool blnFlipVertical = false;
	double dblDegreesOfRotation = 17.0;
	double dblScale = 1.6;

	cv::Mat imgOriginal;							// declare various image variables
	cv::Mat imgGrayscale;
	cv::Mat imgFlipped;
	cv::Mat imgFlippedRotatedAndScaled;

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
	
	if (!blnFlipHorizontal && !blnFlipVertical) {			// no flip
		imgFlipped = imgGrayscale.clone();
	} else if (blnFlipHorizontal && !blnFlipVertical) {			// flip horizontal
		cv::flip(imgGrayscale, imgFlipped, FLIP_HORIZONTAL);
	} else if (!blnFlipHorizontal && blnFlipVertical) {			// flip vertical
		cv::flip(imgGrayscale, imgFlipped, FLIP_VERTICAL);
	} else if (blnFlipHorizontal && blnFlipVertical) {			// flip horizontal and vertical
		cv::flip(imgGrayscale, imgFlipped, FLIP_HORIZONTAL_AND_VERTICAL);
	}

	cv::Mat rotationMatrix = cv::getRotationMatrix2D(cv::Point(imgFlipped.cols / 2, imgFlipped.rows / 2), -dblDegreesOfRotation, dblScale);

	cv::warpAffine(imgFlipped, imgFlippedRotatedAndScaled, rotationMatrix, imgFlipped.size());

	int64 endTickCount = cv::getTickCount();				// end timing
															// calculate and display time
	std::cout << "image processing took " << (endTickCount - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	cv::imshow("imgGrayscale", imgGrayscale);				// show windows
	cv::imshow("imgFlippedRotatedAndScaled", imgFlippedRotatedAndScaled);

	cv::imwrite("imgFlippedRotatedAndScaled.png", imgFlippedRotatedAndScaled);		// write output image to file

	cv::waitKey(0);					// hold windows open until user presses a key

	return(0);
}



