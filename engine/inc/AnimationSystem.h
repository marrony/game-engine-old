/*
 * AnimationSystem.h
 *
 *  Created on: 26/08/2012
 *      Author: marrony
 */

#ifndef ANIMATIONSYSTEM_H_
#define ANIMATIONSYSTEM_H_

#include "math/Vector.h"
#include "math/Matrix.h"
#include "Bone.h"

#include <vector>

namespace engine {

	class Model;
	class Skeleton;
	class Animation;

	class AnimationSystem {
		struct AnimationInput {
			Animation* animation;
			Skeleton* skeleton;
			float currentTime;
		};

		struct AnimationOutput {
			std::vector<math::Matrix4> bonePallete;
		};

		std::vector<AnimationInput> inputs;
		std::vector<AnimationOutput> outputs;
	public:
		void update(float deltaTime) {
			outputs.resize(inputs.size());

			for(size_t i = 0; i < inputs.size(); ++i) {
				AnimationInput& input = inputs[i];
				AnimationOutput& output = outputs[i];

				output.bonePallete.resize(input.skeleton->bones.size());

				for(size_t j = 0; j < input.skeleton->bones.size(); ++j) {
					const std::vector<KeyFrame>& keyFrame = input.animation->keyFrames[j];
					int keyIndex = -1;

					for(size_t i = 0; i < keyFrame.size() - 1; i++) {
						if(frame >= keyFrame[i].time && frame < keyFrame[i+1].time) {
							keyIndex = i;
							break;
						}
					}

					math::Vector3 position(0, 0, 0);
					math::Quaternion rotation(1, 0, 0, 0);

					if(keyIndex == -1) {
						if(frame < keyFrame.front().time) {
							position = keyFrame.front().position;
							rotation = keyFrame.front().rotation;
						} else if(frame >= keyFrame.back().time) {
							position = keyFrame.back().position;
							rotation = keyFrame.back().rotation;
						}
					} else {
						const KeyFrame& frame0 = keyFrame[keyIndex + 0];
						const KeyFrame& frame1 = keyFrame[keyIndex + 1];

						const float a = (frame - frame0.time) / (frame1.time - frame0.time);
						const float b = 1.0 - a;

						position = frame0.position * a + frame1.position * b;
						rotation = math::slerp(frame0.rotation, frame1.rotation, a);
					}

					output.bonePallete[j] = input.skeleton->bones[j].localSkeleton * math::Matrix4::transformationMatrix(rotation, position);
				}
			}
		}
	};

} /* namespace engine */

#endif /* ANIMATIONSYSTEM_H_ */
