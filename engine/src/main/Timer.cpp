/*
 * Timer.cpp
 *
 *  Created on: 01/11/2011
 *      Author: marrony
 */

#include "Timer.h"

namespace engine {

	Timer::Timer() {
#ifdef WIN32
		QueryPerformanceFrequency(&frequency);
		startCount.QuadPart = 0;
		endCount.QuadPart = 0;
#else
		startCount.tv_sec = startCount.tv_usec = 0;
		endCount.tv_sec = endCount.tv_usec = 0;
#endif
	}

	void Timer::start(double microsecs) {
#ifdef WIN32
		QueryPerformanceCounter(&startCount);
		startCount.QuadPart += (microsecs * frequency.QuadPart) / 1000000.0;
#else
		gettimeofday(&startCount, nullptr);
		startCount.tv_usec += microsecs;
#endif
	}

	void Timer::stop() {
#ifdef WIN32
		QueryPerformanceCounter(&endCount);
#else
		gettimeofday(&endCount, nullptr);
#endif
	}

	double Timer::getElapsedTimeInMicroSec() const {
#ifdef WIN32
		QueryPerformanceCounter((LARGE_INTEGER*)&endCount);

		double startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
		double endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
		gettimeofday((timeval*)&endCount, nullptr);

		double startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
		double endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

		return endTimeInMicroSec - startTimeInMicroSec;
	}

	double Timer::getTimeInMicroSec() const {
#ifdef WIN32
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		return counter.QuadPart * (1000000.0 / frequency.QuadPart);
#else
		timeval counter;
		gettimeofday((timeval*)&counter, nullptr);
		return (counter.tv_sec * 1000000.0) + counter.tv_usec;
#endif
	}

} // namespace engine

