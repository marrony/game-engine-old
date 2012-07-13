#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "math/Transformation.h"

using engine::math::Matrix4;
using engine::math::Vector3;
using engine::math::Transformation;
using engine::math::Quaternion;
using engine::math::AxisAngle;
using engine::math::deg2rad;

TEST(Transformation, Identity) {

	Transformation t;

	const float epsilon = 0.001;

	Matrix4 actual = t.createMatrix();
	for(int i = 0; i < 16; i++) {
		ASSERT_NEAR(Matrix4::IDENTITY.matrix[i], actual.matrix[i], epsilon) << "i = " << i;
	}
}

TEST(Transformation, Translation) {

	Transformation t;

	t.translateBy(Vector3(10, 20, 30));

	const float epsilon = 0.001;
	Matrix4 expected = Matrix4::IDENTITY;
	expected.m03 = 10;
	expected.m13 = 20;
	expected.m23 = 30;

	Matrix4 actual = t.createMatrix();
	for(int i = 0; i < 16; i++) {
		ASSERT_NEAR(expected.matrix[i], actual.matrix[i], epsilon) << "i = " << i;
	}
}

TEST(Transformation, Scale) {
	Transformation t;

	t.scaleBy(Vector3(2, 3, 4));

	const float epsilon = 0.001;
	Matrix4 expected = Matrix4::IDENTITY;
	expected.m00 = 2;
	expected.m11 = 3;
	expected.m22 = 4;

	Matrix4 actual = t.createMatrix();
	for(int i = 0; i < 16; i++) {
		ASSERT_NEAR(expected.matrix[i], actual.matrix[i], epsilon) << "i = " << i;
	}
}

TEST(Transformation, Rotation) {
	Transformation t;

	t.rotateBy(Quaternion(AxisAngle(deg2rad(45), Vector3(0, 0, 1))));

	const float epsilon = 0.001;
	Matrix4 expected = Matrix4::IDENTITY;
	expected.m00 = std::cos(deg2rad(45));  expected.m01 = -std::sin(deg2rad(45));
	expected.m10 = std::sin(deg2rad(45));  expected.m11 = std::cos(deg2rad(45));

	Matrix4 actual = t.createMatrix();
	for(int i = 0; i < 16; i++) {
		ASSERT_NEAR(expected.matrix[i], actual.matrix[i], epsilon) << "i = " << i;
	}
}
