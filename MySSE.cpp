/*
The idea of this program is to try out a basic SSE example.

We will declare a huge array, then fill each position with the square root of the index of that position, i.e.

squareRoots[0] == sqrt(0) == 0
. . .
squareRoots[2] == sqrt(2) == 1.41421
. . .
squareRoots[4] == sqrt(4) == 2
. . .
squareRoots[9] == sqrt(9) == 3
. . .
squareRoots[16] == sqrt(16) == 4
. . .
etc.

We will declare and fill an array (of the same size) both without SSE and with SSE, and then compare the timing results
*/

#include<stdlib.h>
#include<iostream>
#include<xmmintrin.h>				// this has the SSE register stuff
#include<opencv2/core/core.hpp>		// this is only included for the OpenCV timing functions, they seem to work much better than the built-in C++ timing functions

// function prototypes ////////////////////////////////////////////////////////////////////////////
double fillSquareRootsArrayWithoutSSE(void);
double fillSquareRootsArrayWithSSE(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	double withoutSSETime;
	double withSSETime;
	
	withoutSSETime = fillSquareRootsArrayWithoutSSE();		// ex. without SSE takes 0.843386 seconds on my computer

	withSSETime = fillSquareRootsArrayWithSSE();			// ex. with SSE takes 0.038295 seconds on my computer

	// ex. with SSE is 22.2028 times faster on my computer, about 20x faster is a significant and worthwhile performance benefit
	std::cout << std::endl << "with SSE was " << withoutSSETime / withSSETime << " times faster" << std::endl << std::endl;

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double fillSquareRootsArrayWithoutSSE(void) {
	std::cout << std::endl << "starting without SSE . . . " << std::endl << std::endl;

	int64 begTickCount = cv::getTickCount();			// get beginning time

	const int arrLength = pow(2, 24);				// array length == 2^24 == 16 777 216

	float *squareRoots = (float*)malloc(arrLength * sizeof(float));		// allocate square roots array

	for (int i = 0; i < arrLength; i++) {
		squareRoots[i] = sqrt((float)i);						// fill square roots array one at a time
	}

	double withoutSSETime = (cv::getTickCount() - begTickCount) / cv::getTickFrequency();		// calculate time taken

	std::cout << "without SSE took = " << withoutSSETime << " seconds" << std::endl << std::endl;		// show time taken

	std::cout << "to prove the function worked, here are the first 10 results:" << std::endl;			// show 1st 10 results to prove function worked
	for (int i = 0; i < 10; i++) {
		std::cout << "squareRoots[" << i << "] = " << squareRoots[i] << std::endl;
	}
	return(withoutSSETime);				// return time taken
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double fillSquareRootsArrayWithSSE(void) {
	std::cout << std::endl << "starting with SSE . . . " << std::endl << std::endl;

	int64 begTickCount = cv::getTickCount();			// get beginning time

	const int arrLength = pow(2, 24);			// array length == 2^24 == 16 777 216

									// squareRoots is 2^24 items long
									// the 16 is b/c variables of type __m128 are automatically aligned on 16-byte boundaries
	float *squareRoots = (float*)_aligned_malloc(arrLength * sizeof(float), 16);

									// declare SSE register variables
	__m128 fourInputs;
	__m128 *squareRootsSSE = (__m128*) squareRoots;
	__m128 fourInputsIncrement = _mm_set_ps(4.0f, 4.0f, 4.0f, 4.0f);

	const int SSELength = arrLength / 4;			// length == 2^24, SSELength == 16 000

	fourInputs = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);	// SSE set packed single

	for (int i = 0; i < SSELength; i++) {				// this loop runs 2^24 / 4 times
		squareRootsSSE[i] = _mm_sqrt_ps(fourInputs);
		fourInputs = _mm_add_ps(fourInputs, fourInputsIncrement);
	}

	double withSSETime = (cv::getTickCount() - begTickCount) / cv::getTickFrequency();			// calculate time taken

	std::cout << "with SSE took = " << withSSETime << " seconds" << std::endl << std::endl;				// show time taken

	std::cout << "to prove the function worked, here are the first 10 results:" << std::endl;			// show 1st 10 results to prove function worked
	for (int i = 0; i < 10; i++) {
		std::cout << "squareRoots[" << i << "] = " << squareRoots[i] << std::endl;
	}
	return(withSSETime);				// return time taken
}




