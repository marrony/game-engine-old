#ifndef MOCKSCENETREEOBJECT_H_
#define MOCKSCENETREEOBJECT_H_

#include "Frustum.h"
#include "ObjectList.h"

using engine::math::Vector3;

class MockSceneTreeObject : public engine::SceneTreeObject {
public:
	MockSceneTreeObject(const Vector3& center, float size) :
			bounding(center, size) {
	}

	virtual engine::AABoundingBox getBoundingBox() const {
		return bounding;
	}

	virtual bool isVisible(const engine::Frustum* const frustum) const {
		return frustum == 0 || frustum->testAgainstBoundingBox(getBoundingBox()) != engine::TestResult::OUT_SIDE;
	}

	virtual void notify() {
	}

	virtual engine::SceneTreeType getSceneTreeType() const {
		return engine::SceneTreeType::GeometrySceneType;
	}
private:
	engine::AABoundingBox bounding;
};

#endif /* MOCKSCENETREEOBJECT_H_ */
