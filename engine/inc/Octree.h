/*
 * Octree.h
 *
 *  Created on: 11/02/2011
 *      Author: marrony.neris
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include "SceneTree.h"
#include "QueryList.h"
#include "math/Vector.h"
#include "OctreeConfiguration.h"

namespace engine {

	class OctreeNode;

	class Octree : public SceneTree {
	public:
		Octree(const math::Vector3& center, float size);
		virtual ~Octree();

		virtual void addObjects(const ObjectList& objectList);
		virtual void addObject(SceneTreeObject* object);
		virtual void queryObjects(QueryList& queryList);
		virtual void queryObjects(ObjectList& queryList, math::Vector3 center, float maxDistance);

		virtual void removeObject(SceneTreeObject* object);

		virtual void maxObjects(int value) {
			configuration.maxObjects(value);
		}

		virtual void minObjects(int value) {
			configuration.minObjects(value);
		}

		virtual void maxLevel(int value) {
			configuration.maxLevel(value);
		}
	private:
		OctreeNode* root;
		OctreeConfiguration configuration;
	};

}  // namespace engine

#endif /* OCTREE_H_ */
