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
#include "Effect.h"

#include <vector>
#include <string>

namespace engine {

	class Buffer;
	class Material;

	struct MeshVertex {
		math::Vector3 position;
		math::Vector3 normal;
		math::Vector3 sTangent;
		math::Vector3 tTangent;
		math::Vector3 color;
		math::Vector2 texCoord;
		math::Vector4 boneIds;
		math::Vector4 weights;

		enum VertexFlags {
			POSITION = 0x01,
			NORMAL = 0x02,
			TEXTURE = 0x04,
			sTANGENT = 0x08,
			tTANGENT = 0x10,
			COLOR = 0x20,
			BONES = 0x40
		};
	};

	struct KeyFrameRotation {
		float time;
		math::Quaternion rotation;
	};

	struct KeyFramePosition {
		float time;
		math::Vector3 position;
	};

	struct Bone {
		std::string name;
		int parentIndex;

		std::vector<KeyFrameRotation> rotations;
		std::vector<KeyFramePosition> positions;

		math::Matrix4 localSkeleton;
		math::Matrix4 globalSkeleton;
	public:
		math::Matrix4 evaluate(float frame) {
			math::Vector3 position(0, 0, 0);
			math::Quaternion rotation(1, 0, 0, 0);

			if(positions.size() > 0) {
				int keyIndex = -1;

				for(size_t i = 0; i < (int)positions.size() - 1; i++) {
					if(frame >= positions[i].time && frame < positions[i+1].time) {
						keyIndex = i;
						break;
					}
				}

				if(keyIndex == -1) {
					if(frame < positions.front().time) {
						position = positions.front().position;
					} else if(frame >= positions.back().time) {
						position = positions.back().position;
					}
				} else {
					const KeyFramePosition& p0 = positions[keyIndex + 0];
					const KeyFramePosition& p1 = positions[keyIndex + 1];

					const float a = (frame - p0.time) / (p1.time - p0.time);
					const float b = 1.0 - a;

					position = p0.position * a + p1.position * b;
				}
			}

			if(rotations.size() > 0) {
				int keyIndex = -1;

				for(size_t i = 0; i < (int)rotations.size() - 1; i++) {
					if(frame >= rotations[i].time && frame < rotations[i+1].time) {
						keyIndex = i;
						break;
					}
				}

				if(keyIndex == -1) {
					if(frame < rotations.front().time) {
						rotation = rotations.front().rotation;
					} else if(frame >= rotations.back().time) {
						rotation = rotations.back().rotation;
					}
				} else {
					const KeyFrameRotation& r0 = rotations[keyIndex + 0];
					const KeyFrameRotation& r1 = rotations[keyIndex + 1];

					const float time = (frame - r0.time) / (r1.time - r0.time);

					rotation = math::slerp(r0.rotation, r1.rotation, time);
				}
			}

			return localSkeleton * math::Matrix4::transformationMatrix(rotation, position);
		}
	};

	class Animation {
		friend class Model;
		friend class ModelUtils;

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

	class GraphicManager;

	struct ModelData {
		ModelData();
		~ModelData();
		void uploadData(GraphicManager* graphicManager);
		void calculateAttributeOffsetsAndElementsPerVertex();
		void calculateTangent();
		void calculateNormal();
		void preCalculateTangent();

		void calculateBoundingBox() {
			aabbox.reset();

			for(const math::Vector3& v : position)
				aabbox.merge(v);
		}

		std::vector<math::Vector3> position;
		std::vector<math::Vector3> normal;
		std::vector<math::Vector3> sTangent;
		std::vector<math::Vector3> tTangent;
		std::vector<math::Vector3> color;
		std::vector<math::Vector2> texCoord;
		std::vector<math::Vector4> boneIds;
		std::vector<math::Vector4> weights;
		std::vector<unsigned short> indices;
		std::vector<math::Matrix4> bindPose;

		int elementsPerVertex;
		int attributeOffsets[MaxAttributeOffset];

		AABoundingBox aabbox;

		GraphicManager* graphicManager;
		int vertexBuffer;
		int indexBuffer;
	};

	struct Mesh {
		unsigned short offset; //offset in indices array
		unsigned short count;  //number os elements to draw
		unsigned short start;  //minimum index in range [offset, offset+count]
		unsigned short end;    //maximum index in range [offset, offset+count]
		Material* material;
		AABoundingBox aabbox;

		class Model* model;
	};

	class Model : public Resource {
	public:
		Model(const std::string& name, ResourceManager* manager);
		~Model();

		void addVertexData(const std::vector<MeshVertex>& vertexArray, const std::vector<unsigned short>& newIndices, Material* material, int flags);

		Mesh* getMesh(size_t index) {
			return meshes[index];
		}

		size_t getMeshCount() const {
			return meshes.size();
		}

		AABoundingBox getBoundingBox() {
			return modelData->aabbox;
		}

		Animation& getAnimation() {
			return animation;
		}

		virtual Type getType() const {
			return TYPE;
		}

		void uploadData(class GraphicManager* graphicManager);

		static const Type TYPE;

		std::vector<Mesh*> meshes;
		ModelData* modelData;
	protected:

	private:
		Animation animation; //TODO separar do model e criar um resource

		friend class ModelUtils;
		friend class ModelEditor;
	};

	struct ModelUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	class ModelKey : public ResourceKey {
	public:
		ModelKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "model/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

} /* namespace engine */

#endif /* MODEL_H_ */
