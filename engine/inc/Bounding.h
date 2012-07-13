#ifndef BOUNDING_H_
#define BOUNDING_H_

#include "math/Vector.h"
#include "math/Plane.h"
#include "math/Matrix.h"

#include <limits>

namespace engine {

	struct TestResult {
		enum Type {
			INTERSECT = 0,
			IN_SIDE   = 1,
			OUT_SIDE  = 2,
		};
	};

	class BoundingCorners {
	public:
		BoundingCorners() {
		}

		BoundingCorners(const math::Vector3 newCorners[8]) {
			for(int i = 0; i < 8; i++) {
				corners[i] = newCorners[i];
			}
		}

		const math::Vector3& operator[](size_t index) const {
			return corners[index];
		}

		math::Vector3& operator[](size_t index) {
			return corners[index];
		}
	private:
		math::Vector3 corners[8];
	};

	class BoundingPlanes {
	public:
		BoundingPlanes(const math::Plane newPlanes[6]) {
			for(int i = 0; i < 6; i++) {
				planes[i] = newPlanes[i];
			}
		}

		TestResult::Type testAgainstSphere(const math::Vector3& center, float radius) const {
			for(int plane = 0; plane < 6; plane++) {
				float distanceOfCenterToPlane = planes[plane].normalize().signedDistance(center);

				if(distanceOfCenterToPlane < -radius) {
					return TestResult::OUT_SIDE;
				}

				if(std::abs(distanceOfCenterToPlane) < radius) {
					return TestResult::INTERSECT;
				}
			}

			return TestResult::IN_SIDE;
		}

		TestResult::Type testAgainstCorners(const BoundingCorners& corners) const {
			int numberOfPlanesThatAllPointsIsInFrontSide = 0;

			for(int plane = 0; plane < 6; plane++) {
				int numberOfPointsNotInBackSide = 8;
				int oneIfAllPointsIsNotInBackSide = 1;

				for(int corner = 0; corner < 8; corner++) {
					if(planes[plane].isInBackSide(corners[corner])) {
						oneIfAllPointsIsNotInBackSide = 0;
						numberOfPointsNotInBackSide--;
					}
				}

				if(numberOfPointsNotInBackSide == 0) {
					return TestResult::OUT_SIDE;
				}

				numberOfPlanesThatAllPointsIsInFrontSide += oneIfAllPointsIsNotInBackSide;
			}

			if(numberOfPlanesThatAllPointsIsInFrontSide == 6) {
				return TestResult::IN_SIDE;
			}

			return TestResult::INTERSECT;
		}

	private:

		math::Plane planes[6];
	};

	class AABoundingBox {
		math::Vector3 min;
		math::Vector3 max;
	public:
		AABoundingBox() {
			reset();
		}

		AABoundingBox(const engine::math::Vector3& center, float size) {
			min = center - size;
			max = center + size;
		}

		AABoundingBox(const math::Vector3& newMin, const math::Vector3& newMax) :
			min(newMin), max(newMax) {
		}

		AABoundingBox transform(const math::Matrix4& matrix) const {
			return AABoundingBox(matrix * min, matrix * max);
		}

		void reset() {
			float maxValue = std::numeric_limits<float>::max();
			float minValue = std::numeric_limits<float>::min();

			min = math::Vector3(maxValue, maxValue, maxValue);
			max = math::Vector3(minValue, minValue, minValue);
		}

		void merge(const AABoundingBox& other) {
			min = math::min(min, other.min);
			max = math::max(max, other.max);
		}

		void merge(const math::Vector3& v) {
			min = math::min(min, v);
			max = math::max(max, v);
		}

		math::Vector3 getCenter() const {
			return (max + min) * 0.5;
		}

		math::Vector3 getMin() const {
			return min;
		}

		math::Vector3 getMax() const {
			return max;
		}

		BoundingCorners getCorners() const {
			const math::Vector3 * const vet[] = { &min, &max };

			const int idxx[] = { 0, 0, 0, 0, 1, 1, 1, 1 };
			const int idxy[] = { 0, 0, 1, 1, 0, 0, 1, 1 };
			const int idxz[] = { 0, 1, 0, 1, 0, 1, 0, 1 };

			BoundingCorners corners;

			for(int i = 0; i < 8; i++) {
				corners[i] = math::Vector3(vet[idxx[i]]->x, vet[idxy[i]]->y, vet[idxz[i]]->z);
			}

			return corners;
		}

		void getPlanes(math::Plane planes[6]) const {
//			planes[0] = math::Plane(corners[2], corners[1], corners[0]);
//			planes[1] = math::Plane(corners[4], corners[5], corners[6]);
			planes[0] = math::Plane(math::Vector3(+1, 0, 0), -min.x);
			planes[1] = math::Plane(math::Vector3(-1, 0, 0), +max.x);

//			planes[2] = math::Plane(corners[0], corners[1], corners[4]);
//			planes[3] = math::Plane(corners[6], corners[3], corners[2]);
			planes[2] = math::Plane(math::Vector3(0, +1, 0), -min.y);
			planes[3] = math::Plane(math::Vector3(0, -1, 0), +max.y);

//			planes[4] = math::Plane(corners[4], corners[2], corners[0]);
//			planes[5] = math::Plane(corners[1], corners[3], corners[5]);
			planes[4] = math::Plane(math::Vector3(0, 0, +1), -min.z);
			planes[5] = math::Plane(math::Vector3(0, 0, -1), +max.z);
		}

		BoundingPlanes getPlanes() const {
			math::Plane planes[6];
			getPlanes(planes);
			return BoundingPlanes(planes);
		}

		TestResult::Type testAgainstBoundingBox(const AABoundingBox& other) const {
			BoundingCorners corners = other.getCorners();

			return getPlanes().testAgainstCorners(corners);
		}
	};

	class BoundingSphere {
		math::Vector3 center;
		float radius;
	public:
		BoundingSphere() {
			reset();
		}

		BoundingSphere(const math::Vector3& newCenter, float newRadius) :
			center(newCenter), radius(newRadius) {
		}

		void reset() {
			center = math::Vector3(0, 0, 0);
			radius = -1;
		}

		math::Vector3 getCenter() const {
			return center;
		}

		float getRadius() const {
			return radius;
		}

		void merge(const BoundingSphere& other) {
			math::Vector3 dir = (other.center - center).normalize();
			math::Vector3 ptActual = center - dir * radius;
			math::Vector3 ptOther = other.center + dir * other.radius;

			math::Vector3 newCenter = (ptActual + ptOther) * 0.5f;

			float newRadius = (newCenter - ptActual).length();

			center = newCenter;
			radius = newRadius;
		}

//		TestResult testAgainst(const Frustum* frustum) const {
//			return frustum->testAgainstSphere(center, radius);
//		}
	};

}  // namespace engine

#endif /* BOUNDING_H_ */
