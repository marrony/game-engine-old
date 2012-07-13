/*
 * OctreeNode.cpp
 *
 *  Created on: 24/03/2011
 *      Author: marrony.neris
 */

#include "OctreeNode.h"
#include "Frustum.h"

namespace engine {

	const math::Vector3 OctreeNode::centers[MAX_OCTANTS] = {
			math::Vector3(-0.5, -0.5, -0.5),
			math::Vector3(-0.5, -0.5, +0.5),
			math::Vector3(-0.5, +0.5, -0.5),
			math::Vector3(-0.5, +0.5, +0.5),
			math::Vector3(+0.5, -0.5, -0.5),
			math::Vector3(+0.5, -0.5, +0.5),
			math::Vector3(+0.5, +0.5, -0.5),
			math::Vector3(+0.5, +0.5, +0.5),
	};

	Octant OctreeNode::determineOctant(const math::Vector3& pt, const math::Vector3& center) {
		int octant = 0;

		if(pt.x > center.x)
			octant |= 4;

		if(pt.y > center.y)
			octant |= 2;

		if(pt.z > center.z)
			octant |= 1;

		return (Octant)octant;
	}

	OctreeNode::OctreeNode(const OctreeConfiguration& newConfiguration, OctreeNode* newParent, const math::Vector3& newCenter, float newSize, int newLevel) :
			configuration(newConfiguration), parent(newParent), center(newCenter), size(newSize), level(newLevel) {
		memset(childs, 0, sizeof(childs));
	}

	OctreeNode::~OctreeNode() {
		for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
			if(childs[i]) {
				delete childs[i];
				childs[i] = 0;
			}
		}
	}

	bool OctreeNode::hasChildren() const {
		for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
			if(childs[i]) {
				return true;
			}
		}
		return false;
	}

	OctreeNode* OctreeNode::getChildAt(Octant index) const {
		return childs[index];
	}

	void OctreeNode::setChildAt(OctreeNode* child, Octant index) {
		childs[index] = child;

		childs[index]->parent = this;
		childs[index]->updateLevel();
	}

	void OctreeNode::updateLevel() {
		if(parent) {
			level = parent->level + 1;

			for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
				if(childs[i]) {
					childs[i]->updateLevel();
				}
			}
		}
	}

	void OctreeNode::queryObjects(QueryList& queryList) {
		const Frustum* frustum = queryList.getFrustum();
		SceneTreeType filter = queryList.getFilter();

		AABoundingBox aabb(center, size);

		if(frustum == 0 || frustum->testAgainstBoundingBox(aabb) != TestResult::OUT_SIDE) {
			for(int i = 0; i < objectList.size(); i++) {
				SceneTreeObject* object = objectList.getObject(i);

				if(filter == object->getSceneTreeType() && object->isVisible(frustum)) {
					queryList.addObject(object);
				}
			}

			for(OctreeNode* child : childs) {
				if(child)
					child->queryObjects(queryList);
			}
		}
	}

	void OctreeNode::queryObjects(ObjectList& queryList, math::Vector3 center, float maxDistance) {
		for(size_t i = 0; i < objectList.size(); ++i) {
			SceneTreeObject* obj = objectList.getObject(i);

			math::Vector3 objCenter = obj->getBoundingBox().getCenter();
			float distance = (objCenter - center).length();

			if(distance <= maxDistance) {
				queryList.addObject(obj);
			}
		}

		for(OctreeNode* child : childs) {
			if(child)
				child->queryObjects(queryList, center, maxDistance);
		}
	}

	ObjectList OctreeNode::getObjectList() const {
		return objectList;
	}

	void OctreeNode::addObjects(const ObjectList& newObjectList) {
		for(size_t i = 0; i < newObjectList.size(); i++) {
			SceneTreeObject* object = newObjectList.getObject(i);

			addObject(object);
		}
	}

	void OctreeNode::addObject(SceneTreeObject* object) {
		Octant octant = determineOctant(object);

		if(octant != INVALID_OCTANT && childs[octant]) {
			childs[octant]->addObject(object);
		} else {
			objectList.addObject(object);
			tryToSplit();
		}
	}

	void OctreeNode::tryToSplit() {
		if(objectList.size() >= configuration.maxObjects() && level < configuration.maxLevel()) {
			ObjectList childObjects[MAX_OCTANTS];

			splitObjects(childObjects);

			for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
				if(!childObjects[i].isEmpty()) {

					if(!childs[i]) {
						childs[i] = createChild((Octant) i);
					}

					childs[i]->addObjects(childObjects[i]);
				}
			}
		}
	}

	void OctreeNode::splitObjects(ObjectList childObjects[MAX_OCTANTS]) {
		std::vector<SceneTreeObject*> removeList;

		for(size_t i = 0; i < objectList.size(); i++) {
			SceneTreeObject* object = objectList.getObject(i);

			Octant octant = determineOctant(object);

			if(octant != INVALID_OCTANT) {
				removeList.push_back(object);
				childObjects[octant].addObject(object);
			}
		}

		for(size_t i = 0; i < removeList.size(); i++) {
			SceneTreeObject* object = removeList[i];
			objectList.removeObject(object);
		}
	}

	void OctreeNode::removeObject(SceneTreeObject* object) {
		if(objectList.contains(object)) {
			objectList.removeObject(object);
			tryToMerge();
		} else {
			Octant octant = determineOctant(object);

			if(childs[octant]) {
				childs[octant]->removeObject(object);
			}
		}
	}

	void OctreeNode::tryToMerge() {
		if(objectsCount() <= configuration.minObjects()) {
			mergeChilds();
		}

		if(parent) {
			parent->tryToMerge();
		}
	}

	size_t OctreeNode::objectsCount() {
		size_t count = objectList.size();

		for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
			if(childs[i]) {
				count += childs[i]->objectsCount();
			}
		}

		return count;
	}

	void OctreeNode::mergeChilds() {
		for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
			if(childs[i]) {
				childs[i]->mergeChilds();

				objectList.addAll(childs[i]->objectList);
				childs[i]->objectList.clear();

				delete childs[i];

				childs[i] = 0;
			}
		}
	}

	bool OctreeNode::isInside(SceneTreeObject* object) const {
		return getBoundingBox().testAgainstBoundingBox(object->getBoundingBox()) == TestResult::IN_SIDE;
	}

	AABoundingBox OctreeNode::getBoundingBox() const {
		return AABoundingBox(center, size);
	}

	OctreeNode* OctreeNode::createEnvolvingNode(Octant octant) {
		BoundingCorners corners = getBoundingBox().getCorners();

		Octant oppositeOctant = (Octant)(7 & ~octant);

		OctreeNode* newNode = new OctreeNode(configuration, 0, corners[octant], size * 2.0, 0);
		newNode->setChildAt(this, oppositeOctant);

		return newNode;
	}

	Octant OctreeNode::determineOctant(SceneTreeObject* object) {
		BoundingCorners corners = object->getBoundingBox().getCorners();

		int octs[MAX_OCTANTS] = { 0 };

		for(int i = OCTANT0; i < MAX_OCTANTS; i++) {
			const Octant octant = determineOctant(corners[i], center);
			octs[octant]++;
		}

		const int* first = octs;
		const int* last = octs + MAX_OCTANTS;
		const int* index = std::find(first, last, MAX_OCTANTS);

		if(index < last) {
			return (Octant)(index - first);
		}

		return INVALID_OCTANT;
	}

	OctreeNode* OctreeNode::createChild(Octant childNumber) {
		math::Vector3 newCenter = center + (centers[childNumber] * size);
		float newSize = size * 0.5;

		return new OctreeNode(configuration, this, newCenter, newSize, level + 1);
	}

} // namespace engine
