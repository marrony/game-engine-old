/*
 * Light.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "math/Vector.h"
#include "math/Matrix.h"
#include "ObjectList.h"

namespace engine {
	class Frustum;
	class Node;

	enum LightType {
		POINT, SPOT, DIRECTIONAL
	};

	class Light : public SceneTreeObject {
		math::Vector3 attenuations;
		math::Vector3 diffuse;
		math::Vector3 specular;
		math::Vector3 ambient;
		Frustum* frustum;
		LightType type;
		bool _castShadow;
		bool enabled;
	public:
		Light(LightType newType, Frustum* newFrustum);
		~Light();

		LightType getType() const;

		bool castShadow() const;
		void castShadow(bool cast);

		Frustum* getFrustum() const;

		math::Vector3 getPosition() const;
		void setPosition(const math::Vector3& newPosition);

		math::Vector3 getDirection() const;
		void setDirection(const math::Vector3& newDirection);

		math::Vector3 getAttenuations() const;
		void setAttenuations(const math::Vector3& newAttenuations);

		/*colors*/
		math::Vector3 getDiffuse() const;
		void setDiffuse(const math::Vector3& newDiffuse);

		math::Vector3 getSpecular() const;
		void setSpecular(const math::Vector3& newSpecular);

		math::Vector3 getAmbient() const;
		void setAmbient(const math::Vector3& newambient);

		math::Vector4 scissorRect(float lightRange, const Frustum* myCamera, const int* viewport) const;

		bool isEnabled() const;
		void enable();
		void disable();

		virtual AABoundingBox getBoundingBox() const {
			return AABoundingBox(getPosition(), 100);
		}

		virtual bool isVisible(const Frustum* const frustum) const;

		virtual void notify() {
		}

		virtual SceneTreeType getSceneTreeType() const {
			return SceneTreeType::LightSceneType;
		}
	};

}  // namespace engine

#endif /* LIGHT_H_ */
