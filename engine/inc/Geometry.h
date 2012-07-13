/*
 * Geometry.h
 *
 *  Created on: 05/05/2011
 *      Author: marrony
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Component.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "Bounding.h"
#include "ObjectList.h"

#include "Resource.h"

#include <vector>

namespace engine {

	class Model;
	class Material;
	class TransformationModifier;
	class ResourceManager;

	class Geometry : public SceneTreeObject, public DataComponent {
	public:
		Geometry(class Model* model, const AABoundingBox& aabbox, TransformationModifier* modifier);
		virtual ~Geometry();

		math::Matrix4 getTransformation() const;

		void setFrame(float frame);

		class Model* getModel() const;

		const math::Matrix4* getBoneMatrix() const;

		virtual void updateBindings(GameEntity* owner);

		TransformationModifier* getModifier() const {
			return modifier;
		}

		void setBoundingBox(AABoundingBox aabbox);

		AABoundingBox getNonTransformedBoundingBox() const;

		AABoundingBox getBoundingBox() const;

		virtual bool isVisible(const Frustum* const frustum) const;

		virtual void notify() {
		}

		virtual SceneTreeType getSceneTreeType() const {
			return SceneTreeType::GeometrySceneType;
		}

		void notifyVisibility(class ResourceManager& manager);
	protected:
		void updateBoundingBox() const;
	private:
		class Model* model;
		mutable AABoundingBox aabbox;
		mutable bool boundingBoxDirty;
		std::vector<math::Matrix4> global;
		TransformationModifier* modifier;
	};

}  // namespace engine

#endif /* GEOMETRY_H_ */
