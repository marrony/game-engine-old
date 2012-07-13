#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "math/Plane.h"

using namespace engine::math;

TEST(Plane, CreateFromTreePoints) {

	const Vector3 v0(0, 0, 1);
	const Vector3 v1(1, 1, 1);
	const Vector3 v2(1, 0, 1);

	const Plane plane(v0, v1, v2);

	const float epsilon = 0.005;

	ASSERT_NEAR(plane.normal.x, 0, epsilon);
	ASSERT_NEAR(plane.normal.y, 0, epsilon);
	ASSERT_NEAR(plane.normal.z, -1, epsilon);
	ASSERT_NEAR(plane.d, 1, epsilon);

	ASSERT_NEAR(plane.signedDistance(v0), 0, epsilon);
	ASSERT_NEAR(plane.signedDistance(v1), 0, epsilon);
	ASSERT_NEAR(plane.signedDistance(v2), 0, epsilon);
}

TEST(Plane, PointIsInFrontSide) {
	const Plane plane(Vector3(1, 0, 0), 0);

	const Vector3 point(1, 0, 0);

	ASSERT_EQ(Plane::FRONT, plane.side(point));
	ASSERT_TRUE(plane.isInFrontSide(point));
}

TEST(Plane, PointIsInBackSide) {
	const Plane plane(Vector3(1, 0, 0), 0);

	const Vector3 point(-1, 0, 0);

	ASSERT_EQ(Plane::BACK, plane.side(point));
	ASSERT_TRUE(plane.isInBackSide(point));
}

TEST(Plane, PointIsInSide) {
	const Plane plane(Vector3(1, 0, 0), -10);

	const Vector3 point(10, 0, 0);

	ASSERT_EQ(Plane::INSIDE, plane.side(point));
	ASSERT_TRUE(plane.isInSide(point));
}
