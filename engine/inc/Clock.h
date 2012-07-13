/*
 * Clock.h
 *
 *  Created on: 31/10/2011
 *      Author: marrony
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "Timer.h"

namespace engine {

	class Clock {
	public:
		Clock();

		void start(double microsecs = 0);
		void tick();

		double getElapsedTimeInMicroSec() const;
		double getTimeInMicroSec() const;
		double getFps() const;
	private:
		Timer timer;
		double elapsedTimeInMicroSec;
		double fps;
	};

	struct GameTime {
		GameTime(double elapsedTimeInMicroSec, double fps) :
			elapsedTimeInMicroSec(elapsedTimeInMicroSec), elapsedTimeInSec(elapsedTimeInMicroSec / 1000000.0), fps(fps) { }

		bool operator==(const GameTime& other) const {
			return elapsedTimeInMicroSec == other.elapsedTimeInMicroSec && fps == other.fps;
		}

		const double elapsedTimeInMicroSec;
		const double elapsedTimeInSec;
		const double fps;
	};

} // namespace engine

#endif /* CLOCK_H_ */
