/*
 * Bone.h
 *
 *  Created on: 08/08/2012
 *      Author: marrony.neris
 */

#ifndef BONE_H_
#define BONE_H_

namespace engine {

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

}  // namespace engine


#endif /* BONE_H_ */
