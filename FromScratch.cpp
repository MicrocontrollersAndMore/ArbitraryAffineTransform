// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

// function prototypes ////////////////////////////////////////////////////////////////////////////
cv::Mat myAffineTransform(cv::Mat transformationMatrix, cv::Mat imgOriginal);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	cv::Mat imgOriginal;		// input image
	cv::Mat imgTransformed;			// Canny edge image

	imgOriginal = cv::imread("test1.png", CV_LOAD_IMAGE_GRAYSCALE);			// open image

	if (imgOriginal.empty()) {									// if unable to open image
		std::cout << "error: image not read from file\n\n";		// show error message on command line
		return(0);												// and exit program
	}

	cv::Mat transformationMatrix(2, 2, CV_64F);

	transformationMatrix.at<double>(0, 0) = 0.5;
	transformationMatrix.at<double>(0, 1) = 0;
	transformationMatrix.at<double>(1, 0) = 0;
	transformationMatrix.at<double>(1, 1) = 0.5;
	
	imgTransformed = myAffineTransform(transformationMatrix, imgOriginal);
	
	cv::imshow("imgOriginal", imgOriginal);		// show windows
	cv::imshow("imgTransformed", imgTransformed);

	cv::waitKey(0);					// hold windows open until user presses a key

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat myAffineTransform(cv::Mat transformationMatrix, cv::Mat imgOriginal) {
	cv::Mat imgTransformed(imgOriginal.rows, imgOriginal.cols, CV_8UC1);

	for (int y = 0; y < imgOriginal.rows; y++) {
		for (int x = 0; x < imgOriginal.cols; x++) {
			int xPrime = (transformationMatrix.at<double>(0, 0) * x) + (transformationMatrix.at<double>(0, 1) * y);
			int yPrime = (transformationMatrix.at<double>(1, 0) * x) + (transformationMatrix.at<double>(1, 1) * y);

			if (x == 344 && y == 101) {
				std::cout << "xPrime = " << xPrime << ", yPrime = " << yPrime << ", imgOriginal.at<uchar>(y, x) = " << imgOriginal.at<uchar>(y, x) << std::endl << std::endl;
			}

			uchar dummy = imgOriginal.at<uchar>(y, x);

			imgTransformed.at<uchar>(yPrime, xPrime) = imgOriginal.at<uchar>(y, x);
		}
	}

	return(imgTransformed);
}






