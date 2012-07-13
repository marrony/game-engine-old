#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Frustum.h"
#include "SpatialLocation.h"
#include "Bounding.h"

using namespace engine;
using namespace engine::math;

static const float epsilon = 0.001;

TEST(Frustum, DefaultPosition) {
	Frustum frustum;

	ASSERT_NEAR(0, frustum.getPosition().x, epsilon);
	ASSERT_NEAR(0, frustum.getPosition().y, epsilon);
	ASSERT_NEAR(0, frustum.getPosition().z, epsilon);
}

TEST(Frustum, DefaultDirection) {
	Frustum frustum;

	ASSERT_NEAR(0, frustum.getDirection().x, epsilon);
	ASSERT_NEAR(0, frustum.getDirection().y, epsilon);
	ASSERT_NEAR(-1, frustum.getDirection().z, epsilon);
}

TEST(Frustum, DefaultNearDistance) {
	Frustum frustum;

	ASSERT_NEAR(0.01, frustum.getNear(), epsilon);
}

TEST(Frustum, DefaultFarDistance) {
	Frustum frustum;

	ASSERT_NEAR(1000.0, frustum.getFar(), epsilon);
}

TEST(Frustum, Undefined) {
	Frustum frustum;

	ASSERT_EQ(FrustumType::UNDEFINED, frustum.getType());
}

TEST(Frustum, Perspective) {
	Frustum frustum;

	frustum.setPerspective(60.0, 1.0);

	ASSERT_EQ(FrustumType::PERSPECTIVE, frustum.getType());
}

TEST(Frustum, Ortho) {
	Frustum frustum;

	frustum.setOrtho(-1, +1, -1, +1);

	ASSERT_EQ(FrustumType::ORTHO, frustum.getType());
}

TEST(Frustum, TestSphereAtOriginOnPerspectiveFrustum) {
	Frustum frustum;

	frustum.setPerspective(60.0, 1.0);

	const Vector3 centerAtOrigin(0, 0, 0);
	const float radius = 10;

	ASSERT_EQ(TestResult::INTERSECT, frustum.testAgainstSphere(centerAtOrigin, radius));
}

TEST(Frustum, TestSphereOutOfPerspectiveFrustum) {
	Frustum frustum;

	frustum.setPerspective(60.0, 1.0);

	const Vector3 center(0, 0, 20);
	const float radius = 10;

	ASSERT_EQ(TestResult::OUT_SIDE, frustum.testAgainstSphere(center, radius));
}

TEST(Frustum, TestSphereInOfPerspectiveFrustum) {
	Frustum frustum;

	frustum.setPerspective(60.0, 1.0);

	const Vector3 center(0, 0, -20);
	const float radius = 10;

	ASSERT_EQ(TestResult::IN_SIDE, frustum.testAgainstSphere(center, radius));
}
