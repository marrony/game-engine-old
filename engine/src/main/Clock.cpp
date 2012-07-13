/*
 * Clock.cpp
 *
 *  Created on: 01/11/2011
 *      Author: marrony
 */

#include "Clock.h"

namespace engine {

	Clock::Clock() :
			timer(), elapsedTimeInMicroSec(), fps() {
	}

	void Clock::start(double microsecs) {
		timer.start(microsecs);
	}

	void Clock::tick() {
		elapsedTimeInMicroSec = timer.getElapsedTimeInMicroSec();
		fps = 1000000.0 / elapsedTimeInMicroSec;
		timer.start();
	}

	double Clock::getElapsedTimeInMicroSec() const {
		return elapsedTimeInMicroSec;
	}

	double Clock::getTimeInMicroSec() const {
		return timer.getTimeInMicroSec();
	}

	double Clock::getFps() const {
		return fps;
	}

} // namespace engine

