/*
 * EulerAngle.h
 *
 *  Created on: 19/07/2011
 *      Author: marrony
 */

#ifndef EULERANGLE_H_
#define EULERANGLE_H_

#include "Vector3.h"

namespace engine {

	namespace math {

		struct EulerAngle {
			float x, y, z;

			EulerAngle() { }

			EulerAngle(float x, float y, float z) :
					x(x), y(y), z(z) { }

			EulerAngle(const float vector[3]) :
					x(vector[0]), y(vector[1]), z(vector[2]) { }

			EulerAngle(const Vector3& vector) :
					x(vector.x), y(vector.y), z(vector.z) { }
		};

	}  // namespace math

}  // namespace engine


#endif /* EULERANGLE_H_ */
