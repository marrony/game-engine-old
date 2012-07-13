/*
 * Timer.h
 *
 *  Created on: 31/10/2011
 *      Author: marrony
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <sys/time.h>

namespace engine {

	class Timer {
	public:
		Timer();

		void start(double microsecs = 0);
		void stop();

		double getElapsedTimeInMicroSec() const;
		double getTimeInMicroSec() const;
	private:
#ifdef WIN32
		LARGE_INTEGER frequency;
		LARGE_INTEGER startCount;
		LARGE_INTEGER endCount;
#else
		timeval startCount;
		timeval endCount;
#endif
	};

} // namespace engine

#endif /* TIMER_H_ */
