/*
 * Frustum.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef FRUSTUM_H_
#define FRUSTUM_H_

#include "math/Vector.h"
#include "math/Matrix.h"
#include "math/Plane.h"
#include "Bounding.h"
#include "Component.h"

namespace engine {

	class SpatialLocation;
	class TransformationModifier;

	enum FrustumType {
		UNDEFINED,
		PERSPECTIVE,
		ORTHO
	};

	class Frustum : public DataComponent {
		void createLookAt() const;
		void createPerspective() const;
		void createOrtho() const;

		void invalidateViewProjectionMatrix();
		void invalidateProjectionMatrix();
		void invalidateViewMatrix();

		void setDefaultPositionAndDirection();
		void setDefaultZNearAndZFar();

		void getPlanes(math::Plane planes[6]) const;
		void getNormalizedPlanes(math::Plane planes[6]) const;
	public:
		Frustum(TransformationModifier* modifier = 0);

		BoundingPlanes getPlanes() const;
		BoundingCorners getCorners() const;

		TestResult::Type testAgainstFrustum(const Frustum* frustum) const;
		TestResult::Type testAgainstSphere(const math::Vector3& center, float radius) const;
		TestResult::Type testAgainstCorners(const BoundingCorners& corners) const;
		TestResult::Type testAgainstBoundingBox(const AABoundingBox& aabb) const;
		TestResult::Type testAgainstBoundingSphere(const BoundingSphere& bsphere) const;

		void setPosition(const engine::math::Vector3& _position);
		engine::math::Vector3 getPosition() const;

		void setDirection(const engine::math::Vector3& _direction);
		engine::math::Vector3 getDirection() const;

		void setPerspective(float newFov, float newAspect);
		void setPerspective(float newFov, float newAspect, float newZNear, float newZFar);

		void setOrtho(float newLeft, float newRight, float newBottom, float newTop);
		void setOrtho(float newLeft, float newRight, float newBottom, float newTop, float newZNear, float newZFar);

		void setAspect(float _aspect);
		float getAspect() const;

		void setFov(float _fov);
		float getFov() const;

		void setNear(float _znear);
		float getNear() const;

		void setFar(float _zfar);
		float getFar() const;

		float getLeft() const;
		float getRight() const;
		float getTop() const;
		float getBottom() const;

		engine::math::Matrix4 getViewMatrix() const;
		engine::math::Matrix4 getProjectionMatrix() const;
		engine::math::Matrix4 getViewProjectionMatrix() const;
		engine::math::Matrix4 getViewProjectionInverseMatrix() const;

		FrustumType getType() const;

		virtual engine::math::Vector3 project(const engine::math::Vector3& obj, const int* viewport) const;
		virtual engine::math::Vector3 unproject(const engine::math::Vector3& win, const int* viewport) const;

		virtual void updateBindings(GameEntity* owner);

		TransformationModifier* getModifier() const {
			return modifier;
		}
	private:
		math::Vector3 direction;
		math::Vector3 position;
		float znear, zfar;
		float aspect, fov;
		float left, right, top, bottom;
		FrustumType type;

		mutable engine::math::Matrix4 view;
		mutable engine::math::Matrix4 projection;

		mutable engine::math::Matrix4 viewProjection;
		mutable engine::math::Matrix4 viewProjectionInverse;

		mutable bool viewInvalid;
		mutable bool projectionInvalid;

		mutable bool viewProjectionInvalid;
		mutable bool viewProjectionInverseInvalid;

		TransformationModifier* modifier;
	};


}  // namespace engine

#endif /* FRUSTUM_H_ */
