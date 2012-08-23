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

	class ResourceManager;
	class Mesh;

	class Animation {
		friend class Mesh;

		std::vector<math::Matrix4> bindPose;
		std::vector<std::vector<KeyFrame>> keyFrames;

		float animationFps;
		float currentTime;
		int totalFrames;
	public:
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
	public:
		Model(const std::string& name, ResourceManager* manager);
		~Model();

		AABoundingBox getBoundingBox();

		Animation& getAnimation();

		virtual Type getType() const {
			return TYPE;
		}

		static const Type TYPE;

		Mesh* geometryData;
		Animation* animation;
		Skeleton* skeleton;
		bool hasAnimation;
	protected:

	private:
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
