/*
 * Plane.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony
 */

#ifndef PLANE_H_
#define PLANE_H_

#include "Vector.h"

namespace engine {

	namespace math {

		class Plane {
		public:
			enum Side {
				INSIDE = 0,
				FRONT  = 1,
				BACK   = 2
			};

			Vector3 normal;
			float d;

			INLINE Plane() {
			}

			INLINE Plane(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
				const Vector3 a0 = v1 - v0;
				const Vector3 a1 = v2 - v0;

				normal = cross(a0, a1).normalize();
				d = -dot(normal, v0);
			}

			INLINE Plane(const Vector3& newNormal, float newD) :
				normal(newNormal), d(newD) {
			}

			INLINE Plane(const Vector4& plane) {
				normal.x = plane.x;
				normal.y = plane.y;
				normal.z = plane.z;
				d = plane.w;
			}

			INLINE operator Vector4() const {
				return Vector4(normal.x, normal.y, normal.z, d);
			}

			INLINE Plane normalize() const {
				float length = normal.length();

				float oneOverLength = 1.0 / length;

				return Plane(normal*oneOverLength, d*oneOverLength);
			}

			INLINE float distance(const Vector3& v) const {
				return std::abs(signedDistance(v));
			}

			INLINE float signedDistance(const Vector3& v) const {
				return normal.x*v.x + normal.y*v.y + normal.z*v.z + d;
			}

			INLINE float distance(const Vector4& v) const {
				return std::abs(signedDistance(v));
			}

			INLINE float signedDistance(const Vector4& v) const {
				return normal.x*v.x + normal.y*v.y + normal.z*v.z + d*v.w;
			}

			INLINE Side side(const Vector3& v) const {
				const float EPSILON = 0.005;

				const float dist = signedDistance(v);

				if(dist > +EPSILON) {
					return FRONT;
				}

				if(dist < -EPSILON) {
					return BACK;
				}

				return INSIDE;
			}

			INLINE bool isInFrontSide(const Vector3& v) const {
				return side(v) == FRONT;
			}

			INLINE bool isInBackSide(const Vector3& v) const {
				return side(v) == BACK;
			}

			INLINE bool isInSide(const Vector3& v) const {
				return side(v) == INSIDE;
			}
		};
	}  // namespace math

}  // namespace engine

#endif /* PLANE_H_ */
