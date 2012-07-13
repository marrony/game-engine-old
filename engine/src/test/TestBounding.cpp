#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Bounding.h"

using engine::math::Vector3;
using engine::AABoundingBox;
using engine::BoundingSphere;

static const float epsilon = 0.005;

TEST(Bounding, TestOutside) {
	engine::AABoundingBox bounding0(Vector3(0, 0, 0), 5);
	engine::AABoundingBox bounding1(Vector3(10, 10, 10), 2);

	ASSERT_EQ(engine::TestResult::OUT_SIDE, bounding0.testAgainstBoundingBox(bounding1));
}

TEST(Bounding, TestInside) {
	engine::AABoundingBox bounding0(Vector3(1, 1, 1), 10);
	engine::AABoundingBox bounding1(Vector3(0, 0, 0), 5);

	ASSERT_EQ(engine::TestResult::IN_SIDE, bounding0.testAgainstBoundingBox(bounding1));
}

TEST(Bounding, TestIntersect) {
	engine::AABoundingBox bounding0(Vector3(0, 0, 0), 5);
	engine::AABoundingBox bounding1(Vector3(5, 5, 5), 3);

	ASSERT_EQ(engine::TestResult::INTERSECT, bounding0.testAgainstBoundingBox(bounding1));
}

TEST(Bounding, TestBug001) {
	engine::AABoundingBox bounding0(Vector3(-100, -100, -100), Vector3(+100, +100, +100));
	engine::AABoundingBox bounding1(Vector3(-332.737, -15.2018995, -325.467987), Vector3(324.583008, 1.17549435e-038, 331.85199));

	ASSERT_EQ(engine::TestResult::INTERSECT, bounding0.testAgainstBoundingBox(bounding1));
}

TEST(Bounding, TestBug002) {
	engine::AABoundingBox bounding0(Vector3(-60000, -60000, -60000), Vector3(+60000, +60000, +60000));
	engine::AABoundingBox bounding1(Vector3(-332.737, -15.2018995, -325.467987), Vector3(324.583008, 1.17549435e-038, 331.85199));

	ASSERT_EQ(engine::TestResult::IN_SIDE, bounding0.testAgainstBoundingBox(bounding1));
}

class GetPlaneFixture : public testing::Test {
public:
	virtual void SetUp() {
		bounding = engine::AABoundingBox(Vector3(0, 0, 0), 5);
		bounding.getPlanes(planes);
	}

	engine::AABoundingBox bounding;
	engine::math::Plane planes[6];
};

TEST_F(GetPlaneFixture, PlaneXPositive) {

	ASSERT_NEAR(planes[0].normal.x, 1.0, epsilon);
	ASSERT_NEAR(planes[0].normal.y, 0.0, epsilon);
	ASSERT_NEAR(planes[0].normal.z, 0.0, epsilon);
	ASSERT_NEAR(planes[0].d, 5.0, epsilon);
}

TEST_F(GetPlaneFixture, PlaneXNegative) {
	ASSERT_NEAR(planes[1].normal.x, -1.0, epsilon);
	ASSERT_NEAR(planes[1].normal.y, 0.0, epsilon);
	ASSERT_NEAR(planes[1].normal.z, 0.0, epsilon);
	ASSERT_NEAR(planes[1].d, 5.0, epsilon);
}

TEST_F(GetPlaneFixture, PlaneYPositive) {

	ASSERT_NEAR(planes[2].normal.x, 0.0, epsilon);
	ASSERT_NEAR(planes[2].normal.y, 1.0, epsilon);
	ASSERT_NEAR(planes[2].normal.z, 0.0, epsilon);
	ASSERT_NEAR(planes[2].d, 5.0, epsilon);
}

TEST_F(GetPlaneFixture, PlaneYNegative) {
	ASSERT_NEAR(planes[3].normal.x, 0.0, epsilon);
	ASSERT_NEAR(planes[3].normal.y, -1.0, epsilon);
	ASSERT_NEAR(planes[3].normal.z, 0.0, epsilon);
	ASSERT_NEAR(planes[3].d, 5.0, epsilon);
}

TEST_F(GetPlaneFixture, PlaneZPositive) {

	ASSERT_NEAR(planes[4].normal.x, 0.0, epsilon);
	ASSERT_NEAR(planes[4].normal.y, 0.0, epsilon);
	ASSERT_NEAR(planes[4].normal.z, 1.0, epsilon);
	ASSERT_NEAR(planes[4].d, 5.0, epsilon);
}

TEST_F(GetPlaneFixture, PlaneZNegative) {
	ASSERT_NEAR(planes[5].normal.x, 0.0, epsilon);
	ASSERT_NEAR(planes[5].normal.y, 0.0, epsilon);
	ASSERT_NEAR(planes[5].normal.z, -1.0, epsilon);
	ASSERT_NEAR(planes[5].d, 5.0, epsilon);
}
