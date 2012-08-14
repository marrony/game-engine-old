/*
 * Model.h
 *
 *  Created on: 13/07/2011
 *      Author: marrony
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "Resource.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "Bounding.h"
#include "Bone.h"
#include "Effect.h"

#include <vector>
#include <string>

namespace engine {

	class Buffer;
	class Material;
	class Model;
	class ResourceManager;

	class Animation {
		friend class ModelBuilder;

		std::vector<Bone> bones;
		float animationFps;
		float currentTime;
		int totalFrames;
	public:
		size_t getBonesCount() const {
			return bones.size();
		}

		void setBoneCount(size_t boneCount) {
			bones.resize(boneCount);
		}

		Bone* getBone(size_t index) {
			return &bones[index];
		}

		Bone* findBone(const std::string& name) {
			for(size_t i = 0; i < bones.size(); i++) {
				if(bones[i].name == name) {
					return &bones[i];
				}
			}

			return 0;
		}

		/**
		 * globalSkeleton = parent->globalSkeleton * localSkeleton;
		 *
		 * local = localSkeleton * animateMatrix;
		 * global = parent->global * local;
		 *
		 * v0 = globalSkeleton^-1 * vertex; <== constante
		 * vFinal = global * v0;
		 */
		void updateBones() {
			for(size_t i = 0; i < bones.size(); i++) {
				if(bones[i].parentIndex != -1) {
					Bone* parent = &bones[bones[i].parentIndex];

					bones[i].globalSkeleton = parent->globalSkeleton * bones[i].localSkeleton;
				} else {
					bones[i].globalSkeleton = bones[i].localSkeleton;
				}
			}
		}

		void setFrameProperties(float animationFps, int totalFrames) {
			this->currentTime = -1;
			this->animationFps = animationFps;
			this->totalFrames = totalFrames;
		}
	};

	struct Batch {
		unsigned short offset; //offset in indices array
		unsigned short count;  //number os elements to draw
		unsigned short start;  //minimum index in range [offset, offset+count]
		unsigned short end;    //maximum index in range [offset, offset+count]
	};

	class Model : public Resource {
		friend class ModelBuilder;
	public:
		Model(const std::string& name, ResourceManager* manager);
		~Model();

		Batch* getBatch(size_t index) {
			return &batches[index];
		}

		Material* getMaterial(size_t index) {
			return materials[index];
		}

		size_t getBatchCount() const {
			return batches.size();
		}

		AABoundingBox getBoundingBox() {
			return aabbox;
		}

		Animation& getAnimation() {
			return animation;
		}

		virtual Type getType() const {
			return TYPE;
		}

		void load(const std::string& filename);

		static const Type TYPE;

		std::vector<Batch> batches;
		std::vector<Material*> materials;
		std::vector<AABoundingBox> aabboxes;
		AABoundingBox aabbox;
		int elementsPerVertex;
		bool hasAnimation;
		int attributeOffsets[MaxAttributeOffset];
		Buffer* vertexBuffer;
		Buffer* indexBuffer;
	protected:

	private:
		Animation animation; //TODO separar do model e criar um resource
	};

	class ModelKey : public ResourceKey {
	public:
		ModelKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "model/" + getName();
		}

		virtual Resource* loadResource(ResourceManager& manager) const;
	};

} /* namespace engine */

#endif /* MODEL_H_ */
