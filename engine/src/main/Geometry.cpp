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

	ModelInstance::ModelInstance(Model* model, const AABoundingBox& aabbox, TransformationModifier* modifier) :
		model(model), modifier(modifier), boundingBoxDirty(false), aabbox(aabbox) {
	}

	ModelInstance::~ModelInstance() {
		model->getManager()->unloadResource(model);
	}

	AABoundingBox ModelInstance::getNonTransformedBoundingBox() const {
		if(boundingBoxDirty) {
			updateBoundingBox();
			boundingBoxDirty = false;
		}

		return aabbox;
	}

	AABoundingBox ModelInstance::getBoundingBox() const {
		return getNonTransformedBoundingBox().transform(getTransformation());
	}

	void ModelInstance::setBoundingBox(AABoundingBox aabbox) {
		this->aabbox = aabbox;
		boundingBoxDirty = false;
	}

	math::Matrix4 ModelInstance::getTransformation() const {
		if(modifier != 0) {
			return modifier->getTransformation();
		}

		return math::Matrix4::IDENTITY;
	}

	void ModelInstance::setFrame(float frame) {
		if(!model) return;

		boundingBoxDirty = true;

		if(frame < 0) {
			for(size_t i = 0; i < model->skeleton->getBonesCount(); i++) {
				global[i] = model->skeleton->getBone(i)->globalSkeleton;
			}
		} else {
			for(size_t i = 0; i < model->skeleton->getBonesCount(); i++) {
				Bone* bone = model->skeleton->getBone(i);

				if(bone->parentIndex != -1) {
					global[i] = global[bone->parentIndex] * bone->evaluate(frame);
				} else {
					global[i] = bone->evaluate(frame);
				}
			}
		}
	}

	Model* ModelInstance::getModel() const {
		return model;
	}

	const math::Matrix4* ModelInstance::getBoneMatrix() const {
		return global.data();
	}

	void ModelInstance::updateBindings(GameEntity* owner) {
	}

	void ModelInstance::updateBoundingBox() const {
		if(!model) return;

		aabbox.reset();
#if 0
		for(size_t i = 0; i < model->modelData->position.size(); i++) {
			math::Vector3 myVertex = model->modelData->position[i];

			math::Vector3 position;

			if(!model->modelData->boneIds.empty() && !model->modelData->weights.empty() && !global.empty()) {
				position = {0, 0, 0};

				math::Vector4 myBoneIds = model->modelData->boneIds[i];
				math::Vector4 myWeights = model->modelData->weights[i];

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
#endif
	}

	void ModelInstance::notifyVisibility(ResourceManager& manager) {
		if(model) {
			//global.resize(model->getAnimation().getBonesCount(), math::Matrix4::IDENTITY);
		}
	}

	bool ModelInstance::isVisible(const Frustum* const frustum) const {
		return frustum == 0 || frustum->testAgainstBoundingBox(getBoundingBox()) != TestResult::OUT_SIDE;
	}

}  // namespace engine

