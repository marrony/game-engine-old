/*
 * AxisAngle.h
 *
 *  Created on: 31/01/2011
 *      Author: marrony
 */

#ifndef AXISANGLE_H_
#define AXISANGLE_H_

#include "Vector.h"

namespace engine {

	namespace math {

		/**
		 * http://en.wikipedia.org/wiki/Axis_angle
		 */
		struct AxisAngle {
			float angle;
			Vector3 axis;

			AxisAngle(float angle, const Vector3& axis) :
					angle(angle), axis(axis) { }
		};

	}  // namespace math

}  // namespace engine

#endif /* AXISANGLE_H_ */
