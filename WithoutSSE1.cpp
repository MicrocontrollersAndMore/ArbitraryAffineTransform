#include <stdlib.h>
#include <iostream>
#include <xmmintrin.h>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	std::cout << "starting calculation . . . " << std::endl << std::endl;

	int64 begTickCount = cv::getTickCount();			// get beginning time

	const int length = 64000;

						// we will be calculating Y = sqrt(x) / x, for x = 1 to 64000
						// if you do not properly align your data for SSE instructions, you may take a huge performance hit.

									// length == 64 000
									// sizeof(float) == 4
									//
									// length * sizeof(float) = 256 000
									// 256 000 / 16 = 16 000

									// somehow results ends up being 64,000 items long ??
	//float *results = (float*)_aligned_malloc(length * sizeof(float), 16);			// align to 16-byte for SSE

	float results[64000];

	for (int stress = 0; stress < 100; stress++)	{			// lots of stress loops so we can easily use a stopwatch
		float xFloat = 1.0f;
		
		for (int i = 0; i < length; i++) {			// this loop runs 64 000 times
			results[i] = sqrt(xFloat) / xFloat;		// even though division is slow, there are no intrinsic functions like there are in SSE
			xFloat += 1.0f;
		}
	}
								// calculate and show time taken
	std::cout << "calculation took = " << (cv::getTickCount() - begTickCount) / cv::getTickFrequency() << " seconds" << std::endl << std::endl;

	std::cout << std::endl << std::endl << "to prove program worked, here are the first 20 results" << std::endl;
	for (int i = 0; i < 20; i++) {
		std::cout << "results[i] = " << results[i] << std::endl;
	}

	return 0;
}


