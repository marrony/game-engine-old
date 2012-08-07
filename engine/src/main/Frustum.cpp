/*
 * Frustum.cpp
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#include "Frustum.h"
#include "GameEntity.h"
#include "SpatialLocation.h"
#include "TransformationModifier.h"

namespace engine {

	void Frustum::createLookAt() const {
		view = math::Matrix4::lookAtMatrix(position, direction);

		if(modifier != 0)
			view = modifier->getTransformation() * view;
	}

	void Frustum::createPerspective() const {
		projection = math::Matrix4::perspectiveMatrix(fov, aspect, znear, zfar);
	}

	void Frustum::createOrtho() const {
		projection = math::Matrix4::orthoMatrix(left, right, bottom, top, znear, zfar);
	}

	void Frustum::invalidateViewProjectionMatrix() {
		viewProjectionInvalid = true;
		viewProjectionInverseInvalid = true;
	}

	void Frustum::invalidateProjectionMatrix() {
		projectionInvalid = true;
		invalidateViewProjectionMatrix();
	}

	void Frustum::invalidateViewMatrix() {
		viewInvalid = true;
		invalidateViewProjectionMatrix();
	}

	void Frustum::setDefaultPositionAndDirection() {
		setDirection(math::Vector3(0, 0, -1));
		setPosition(math::Vector3(0, 0, 0));
	}

	void Frustum::setDefaultZNearAndZFar() {
		znear = 0.01;
		zfar = 1000.0;
	}

	Frustum::Frustum(TransformationModifier* modifier) :
			modifier(modifier), type(FrustumType::UNDEFINED) {
		setDefaultPositionAndDirection();
		setDefaultZNearAndZFar();
	}

	BoundingPlanes Frustum::getPlanes() const {
		math::Plane planes[6];
		getPlanes(planes);

		return BoundingPlanes(planes);
	}

	void Frustum::getPlanes(math::Plane planes[6]) const {
		math::Vector4 row0 = this->getViewProjectionMatrix().row(0);
		math::Vector4 row1 = this->getViewProjectionMatrix().row(1);
		math::Vector4 row2 = this->getViewProjectionMatrix().row(2);
		math::Vector4 row3 = this->getViewProjectionMatrix().row(3);

		planes[0] = row0 + row3;
		planes[1] = row3 - row0;

		planes[2] = row1 + row3;
		planes[3] = row3 - row1;

		planes[4] = row2 + row3;
		planes[5] = row3 - row2;
	}

	void Frustum::getNormalizedPlanes(math::Plane planes[6]) const {
		math::Plane planesTemp[6];

		getPlanes(planesTemp);

		for(int i = 0; i < 6; i++) {
			planes[i] = planesTemp[i].normalize();
		}
	}

	BoundingCorners Frustum::getCorners() const {
		BoundingCorners corners;

		math::Vector3 right, up, d;

		d = direction.normalize();
		if(1.0 - std::abs(d.y) <= 0.005) {
			right.x = 1.0;
			right.y = 0.0;
			right.z = 0.0;
		} else {
			right = math::cross(math::Vector3(0, 1, 0), d);
		}

		up = math::cross(d, right);

		float tanfov2 = std::tan(math::deg2rad(fov / 2.0));

		math::Vector3 vfar = position + (d * zfar);
		float hfar2 = zfar * tanfov2;
		float wfar2 = hfar2 * aspect;

		corners[0] = vfar - (up * hfar2) - (right * wfar2);
		corners[1] = vfar + (up * hfar2) - (right * wfar2);
		corners[2] = vfar + (up * hfar2) + (right * wfar2);
		corners[3] = vfar - (up * hfar2) + (right * wfar2);

		math::Vector3 vnear = position + (d * znear);
		float hnear2 = znear * tanfov2;
		float wnear2 = hnear2 * aspect;

		corners[4] = vnear - (up * hnear2) - (right * wnear2);
		corners[5] = vnear + (up * hnear2) - (right * wnear2);
		corners[6] = vnear + (up * hnear2) + (right * wnear2);
		corners[7] = vnear - (up * hnear2) + (right * wnear2);

		return corners;
	}

	TestResult::Type Frustum::testAgainstFrustum(const Frustum* frustum) const {
		return testAgainstCorners(frustum->getCorners());
	}

	TestResult::Type Frustum::testAgainstSphere(const math::Vector3& center, float radius) const {
		return getPlanes().testAgainstSphere(center, radius);
	}

	TestResult::Type Frustum::testAgainstCorners(const BoundingCorners& corners) const {
		return getPlanes().testAgainstCorners(corners);
	}

	TestResult::Type Frustum::testAgainstBoundingBox(const AABoundingBox& aabb) const {
		return testAgainstCorners(aabb.getCorners());
	}

	TestResult::Type Frustum::testAgainstBoundingSphere(const BoundingSphere& bsphere) const {
		return testAgainstSphere(bsphere.getCenter(), bsphere.getRadius());
	}

	void Frustum::setPosition(const engine::math::Vector3& newPosition) {
		position = newPosition;

		invalidateViewMatrix();
	}

	engine::math::Vector3 Frustum::getPosition() const {
		return position;
	}

	void Frustum::setDirection(const engine::math::Vector3& _direction) {
		direction = _direction;

		invalidateViewMatrix();
	}

	engine::math::Vector3 Frustum::getDirection() const {
		return direction;
	}

	void Frustum::setPerspective(float newFov, float newAspect) {
		setPerspective(newFov, newAspect, znear, zfar);
	}

	void Frustum::setPerspective(float newFov, float newAspect, float newZNear, float newZFar) {
		fov = newFov;
		aspect = newAspect;
		znear = newZNear;
		zfar = newZFar;
		type = PERSPECTIVE;

		invalidateProjectionMatrix();
	}

	void Frustum::setOrtho(float newLeft, float newRight, float newBottom, float newTop) {
		setOrtho(newLeft, newRight, newBottom, newTop, znear, zfar);
	}

	void Frustum::setOrtho(float newLeft, float newRight, float newBottom, float newTop, float newZNear, float newZFar) {
		left = newLeft;
		right = newRight;
		bottom = newBottom;
		top = newTop;
		znear = newZNear;
		zfar = newZFar;
		type = ORTHO;

		invalidateProjectionMatrix();
	}

	void Frustum::setAspect(float _aspect) {
		aspect = _aspect;

		invalidateProjectionMatrix();
	}

	float Frustum::getAspect() const {
		return aspect;
	}

	void Frustum::setFov(float _fov) {
		fov = _fov;

		invalidateProjectionMatrix();
	}

	float Frustum::getFov() const {
		return fov;
	}

	void Frustum::setNear(float _znear) {
		znear = _znear;

		invalidateProjectionMatrix();
	}

	float Frustum::getNear() const {
		return znear;
	}

	void Frustum::setFar(float _zfar) {
		zfar = _zfar;

		invalidateProjectionMatrix();
	}

	float Frustum::getFar() const {
		return zfar;
	}

	float Frustum::getLeft() const {
		return left;
	}

	float Frustum::getRight() const {
		return right;
	}

	float Frustum::getTop() const {
		return top;
	}

	float Frustum::getBottom() const {
		return bottom;
	}

	engine::math::Matrix4 Frustum::getViewMatrix() const {
		if(viewInvalid) {
			viewInvalid = false;

			createLookAt();
		}
		return view;
	}

	engine::math::Matrix4 Frustum::getProjectionMatrix() const {
		if(projectionInvalid) {
			projectionInvalid = false;

			switch(type) {
			case PERSPECTIVE:
				createPerspective();
				break;

			case ORTHO:
				createOrtho();
				break;

			default:
				break;
			}
		}

		return projection;
	}

	engine::math::Matrix4 Frustum::getViewProjectionMatrix() const {
		if(viewProjectionInvalid) {
			viewProjectionInvalid = false;

			viewProjection = getProjectionMatrix() * getViewMatrix();
		}
		return viewProjection;
	}

	engine::math::Matrix4 Frustum::getViewProjectionInverseMatrix() const {
		if(viewProjectionInverseInvalid) {
			viewProjectionInverseInvalid = false;

			viewProjectionInverse = getViewProjectionMatrix().inverse();
		}
		return viewProjectionInverse;
	}

	FrustumType Frustum::getType() const {
		return type;
	}

	engine::math::Vector3 Frustum::project(const engine::math::Vector3& obj, const int* viewport) const {
		engine::math::Vector4 v = getViewProjectionMatrix() * engine::math::Vector4(obj.x, obj.y, obj.z, 1.0f);

		engine::math::Vector3 result(v.x / v.w, v.y / v.w, v.z / v.w);

		result = result * 0.5f + 0.5f;

		result.x = result.x * viewport[2] + viewport[0];
		result.y = result.y * viewport[3] + viewport[1];

		return result;
	}

	engine::math::Vector3 Frustum::unproject(const engine::math::Vector3& win, const int* viewport) const {
		engine::math::Vector4 objRange;
		engine::math::Vector4 obj;

		engine::math::Matrix4 modelViewProjectionInverse = getViewProjectionInverseMatrix();

		objRange.x = 2.0 * ((win.x - viewport[0]) / viewport[2]) - 1.0;
		objRange.y = 2.0 * ((win.y - viewport[1]) / viewport[3]) - 1.0;
		objRange.z = 2.0 * win.z - 1.0;
		objRange.w = 1.0;

		obj = modelViewProjectionInverse * objRange;

//		if(obj.w == 0) {
//			throw engine::Exception();
//		}

		return engine::math::Vector3(obj.x / obj.w, obj.y / obj.w, obj.z / obj.w);
	}

	void Frustum::updateBindings(GameEntity* owner) {
		modifier = owner->getComponent<TransformationModifier>();
	}

}  // namespace engine
