#include <stdlib.h>
#include <iostream>
#include <xmmintrin.h>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	std::cout << "starting calculation . . . " << std::endl << std::endl;

	int64 begTickCount = cv::getTickCount();

	const int length = 64000;

						// We will be calculating Y = sqrt(x) / x, for x = 1->64000
						// If you do not properly align your data for SSE instructions, you may take a huge performance hit.

						// result is 64 000 items long
						// the 16 is because variables of type _m128 are automatically aligned on 16-byte boundaries
	float *pResult = (float*)_aligned_malloc(length * sizeof(float), 16);

	__m128 x;
					// SSE set packed single, "set1" sets all 4 variables to the same value
	__m128 xDelta = _mm_set1_ps(4.0f);		// Set the xDelta to (4, 4, 4, 4)
	__m128 *pResultSSE = (__m128*) pResult;

	const int SSELength = length / 4;			// length == 64 000, SSELength == 16 000

	for (int stress = 0; stress < 100; stress++)	 {				// lots of stress loops so we can easily use a stopwatch
		
														// SSE set packed single
		x = _mm_set_ps(4.0f, 3.0f, 2.0f, 1.0f);			// Set the initial values of x to (4, 3, 2, 1)

		for (int i = 0; i < SSELength; i++) {			// this loop runs 16 000 times
			
								// SSE square root packed single, computes the square roots of four single-precision floating-point values

			__m128 xSqrt = _mm_sqrt_ps(x);

			pResultSSE[i] = _mm_div_ps(xSqrt, x);
								// NOTE! Sometimes, the order in which things are done in SSE may seem reversed.
								// When the command above executes, the four floating elements are actually flipped around
								// We have already compensated for that flipping by setting the initial x vector to (4,3,2,1) instead of (1,2,3,4)
			x = _mm_add_ps(x, xDelta);	// Advance x to the next set of numbers
		}
	}

	std::cout << "calculation took = " << (cv::getTickCount() - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	std::cout << std::endl << std::endl << "to prove program worked, here are the first 20 results" << std::endl;
	for (int i = 0; i < 20; i++) {
		std::cout << "results[i] = " << pResult[i] << std::endl;
	}

	return(0);
}


