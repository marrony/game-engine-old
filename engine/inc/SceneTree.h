/*
 * SceneTree.h
 *
 *  Created on: 11/02/2011
 *      Author: marrony.neris
 */

#ifndef SCENETREE_H_
#define SCENETREE_H_

#include "math/Vector.h"

namespace engine {

	class QueryList;
	class ObjectList;
	class SceneTreeObject;

	struct SceneTree {
		virtual ~SceneTree() { }

		virtual void queryObjects(QueryList& queryList) = 0;
		virtual void queryObjects(ObjectList& queryList, math::Vector3 center, float maxDistance) = 0;

		virtual void addObjects(const ObjectList& objectList) = 0;
		virtual void addObject(SceneTreeObject* object) = 0;
		virtual void removeObject(SceneTreeObject* object) = 0;

		virtual void maxObjects(int value) = 0;
		virtual void minObjects(int value) = 0;
		virtual void maxLevel(int value) = 0;
	};

}  // namespace engine

#endif /* SCENETREE_H_ */
