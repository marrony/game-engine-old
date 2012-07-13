/*
 * Geometry.cpp
 *
 *  Created on: 05/05/2011
 *      Author: marrony
 */

#include "Geometry.h"
#include "GameEntity.h"
#include "Material.h"
#include "Model.h"
#include "Node.h"
#include "Frustum.h"
#include "ResourceManager.h"

namespace engine {

	Geometry::Geometry(Model* model, const AABoundingBox& aabbox, TransformationModifier* modifier) :
		model(model), modifier(modifier), boundingBoxDirty(false), aabbox(aabbox) {
	}

	Geometry::~Geometry() {
	}

	AABoundingBox Geometry::getNonTransformedBoundingBox() const {
		if(boundingBoxDirty) {
			updateBoundingBox();
			boundingBoxDirty = false;
		}

		return aabbox;
	}

	AABoundingBox Geometry::getBoundingBox() const {
		return getNonTransformedBoundingBox().transform(getTransformation());
	}

	void Geometry::setBoundingBox(AABoundingBox aabbox) {
		this->aabbox = aabbox;
		boundingBoxDirty = false;
	}

	math::Matrix4 Geometry::getTransformation() const {
		if(modifier != 0) {
			return modifier->getTransformation();
		}

		return math::Matrix4::IDENTITY;
	}

	void Geometry::setFrame(float frame) {
		if(!model) return;

		boundingBoxDirty = true;

		if(frame < 0) {
			for(size_t i = 0; i < model->getAnimation().getBonesCount(); i++) {
				global[i] = model->getAnimation().getBone(i)->globalSkeleton;
			}
		} else {
			for(size_t i = 0; i < model->getAnimation().getBonesCount(); i++) {
				Bone* bone = model->getAnimation().getBone(i);

				if(bone->parentIndex != -1) {
					global[i] = global[bone->parentIndex] * bone->evaluate(frame);
				} else {
					global[i] = bone->evaluate(frame);
				}
			}
		}
	}

	Model* Geometry::getModel() const {
		return model;
	}

	const math::Matrix4* Geometry::getBoneMatrix() const {
		return global.data();
	}

	void Geometry::updateBindings(GameEntity* owner) {
	}

	void Geometry::updateBoundingBox() const {
		if(!model) return;

		aabbox.reset();

		for(size_t i = 0; i < model->geometry->position.size(); i++) {
			math::Vector3 myVertex = model->geometry->position[i];

			math::Vector3 position;

			if(!model->geometry->boneIds.empty() && !model->geometry->weights.empty() && !global.empty()) {
				position = {0, 0, 0};

				math::Vector4 myBoneIds = model->geometry->boneIds[i];
				math::Vector4 myWeights = model->geometry->weights[i];

				int boneId = int(myBoneIds.x);
				if(boneId >= 0) {
					position += (global[boneId] * myVertex) * myWeights.x;
				}

				boneId = int(myBoneIds.y);
				if(boneId > 0) {
					position += (global[boneId] * myVertex) * myWeights.y;
				}
			} else {
				position = myVertex;
			}

			aabbox.merge(position);
		}
	}

	void Geometry::notifyVisibility(ResourceManager& manager) {
		if(model) {
			global.resize(model->getAnimation().getBonesCount(), math::Matrix4::IDENTITY);
		}
	}

	bool Geometry::isVisible(const Frustum* const frustum) const {
		return frustum == 0 || frustum->testAgainstBoundingBox(getBoundingBox()) != TestResult::OUT_SIDE;
	}

}  // namespace engine

