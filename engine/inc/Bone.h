/*
 * Bone.h
 *
 *  Created on: 08/08/2012
 *      Author: marrony.neris
 */

#ifndef BONE_H_
#define BONE_H_

namespace engine {

	struct KeyFrame {
		float time;
		math::Quaternion rotation;
		math::Vector3 position;
	};

	struct Bone {
		std::string name;
		int parentIndex;

		std::vector<KeyFrame> keyframes;

		math::Matrix4 localSkeleton;
		math::Matrix4 globalSkeleton;
	public:
		math::Matrix4 evaluate(float frame) {
			math::Vector3 position(0, 0, 0);
			math::Quaternion rotation(1, 0, 0, 0);

			if(keyframes.size() > 0) {
				int keyIndex = -1;

				for(size_t i = 0; i < (int)keyframes.size() - 1; i++) {
					if(frame >= keyframes[i].time && frame < keyframes[i+1].time) {
						keyIndex = i;
						break;
					}
				}

				if(keyIndex == -1) {
					if(frame < keyframes.front().time) {
						position = keyframes.front().position;
					} else if(frame >= keyframes.back().time) {
						position = keyframes.back().position;
					}
				} else {
					const KeyFrame& frame0 = keyframes[keyIndex + 0];
					const KeyFrame& frame1 = keyframes[keyIndex + 1];

					const float a = (frame - frame0.time) / (frame1.time - frame0.time);
					const float b = 1.0 - a;

					position = frame0.position * a + frame1.position * b;
					rotation = math::slerp(frame0.rotation, frame1.rotation, a);
				}
			}

			return localSkeleton * math::Matrix4::transformationMatrix(rotation, position);
		}
	};

	class Skeleton {
	public:
		std::vector<Bone> bones;

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
	};

}  // namespace engine


#endif /* BONE_H_ */
