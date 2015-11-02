// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

// function prototypes ////////////////////////////////////////////////////////////////////////////
cv::Mat myAffineTransform(cv::Mat transformationMatrix, cv::Mat imgOriginal);
cv::Mat getTransformationMatrix(cv::Point2f p2fCenter, double dblAngleInDegrees, double dblScale);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	cv::Mat imgOriginal;			// input image
	cv::Mat imgTransformed;			// Canny edge image

	imgOriginal = cv::imread("lena.pgm", CV_LOAD_IMAGE_GRAYSCALE);			// open image

	if (imgOriginal.empty()) {									// if unable to open image
		std::cout << "error: image not read from file\n\n";		// show error message on command line
		return(0);												// and exit program
	}

	double dblDegreesOfRotation = 47.0;
	double dblScale = 1.0;

	cv::Mat transformationMatrix = getTransformationMatrix(cv::Point(imgOriginal.cols / 2, imgOriginal.rows / 2), dblDegreesOfRotation, dblScale);

	std::cout << transformationMatrix << std::endl;

	imgTransformed = myAffineTransform(transformationMatrix, imgOriginal);
	
	cv::imshow("imgOriginal", imgOriginal);		// show windows
	cv::imshow("imgTransformed", imgTransformed);

	cv::waitKey(0);					// hold windows open until user presses a key

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat getTransformationMatrix(cv::Point2f p2fCenter, double dblAngleInDegrees, double dblScale) {
	double dblAngleInRadians = dblAngleInDegrees * (3.14159 / 180);

	double dblAlpha = cos(dblAngleInRadians) * dblScale;
	double dblBeta = sin(dblAngleInRadians) * dblScale;

	cv::Mat transformationMatrix(2, 3, CV_64F);

	transformationMatrix.at<double>(0, 0) = dblAlpha;
	transformationMatrix.at<double>(0, 1) = dblBeta;
	transformationMatrix.at<double>(0, 2) = ((1 - dblAlpha) * p2fCenter.x) - (dblBeta * p2fCenter.y);
	transformationMatrix.at<double>(1, 0) = -dblBeta;
	transformationMatrix.at<double>(1, 1) = dblAlpha;
	transformationMatrix.at<double>(1, 2) = (dblBeta * p2fCenter.x) + ((1 - dblAlpha) * p2fCenter.y);

	return(transformationMatrix);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat myAffineTransform(cv::Mat transformationMatrix, cv::Mat imgOriginal) {
	cv::Mat imgTransformed(imgOriginal.rows, imgOriginal.cols, CV_8UC1);

	for (int x = 0; x < imgOriginal.cols; x++) {
		for (int y = 0; y < imgOriginal.rows; y++) {
			int xPrime = (int)((transformationMatrix.at<double>(0, 0) * x) + (transformationMatrix.at<double>(0, 1) * y) + transformationMatrix.at<double>(0, 2));
			int yPrime = (int)((transformationMatrix.at<double>(1, 0) * x) + (transformationMatrix.at<double>(1, 1) * y) + transformationMatrix.at<double>(1, 2));

			if (xPrime >= 0 && xPrime < imgTransformed.cols && yPrime >= 0 && yPrime < imgTransformed.rows) {
				imgTransformed.at<uchar>(yPrime, xPrime) = imgOriginal.at<uchar>(y, x);
			}

		}
	}

	return(imgTransformed);
}






