/*
 * Matrix34.h
 *
 *  Created on: 20/07/2011
 *      Author: marrony
 */

#ifndef MATRIX34_H_
#define MATRIX34_H_

#include "Vector4.h"

namespace engine {

	namespace math {

		/**
		 * m00, m01, m02, m03
		 * m10, m11, m12, m13
		 * m20, m21, m22, m23
		 */
		struct Matrix34 {
			Vector4 m[3];
		};

	}  // namespace math

}  // namespace engine


#endif /* MATRIX34_H_ */
