/*
 * Octree.cpp
 *
 *  Created on: 11/02/2011
 *      Author: marrony.neris
 */

#include "Octree.h"
#include "OctreeNode.h"
#include "Frustum.h"

namespace engine {

	Octree::Octree(const math::Vector3& center, float size) {
		configuration.maxLevel(5);
		configuration.maxObjects(3);
		configuration.minObjects(2);
		root = new OctreeNode(configuration, 0, center, size, 0);
	}

	Octree::~Octree() {
		delete root;
	}

	void Octree::addObjects(const ObjectList& objectList) {
		root->addObjects(objectList);
	}

	void Octree::addObject(SceneTreeObject* object) {
		//todo corrigir bug
		if(!root->isInside(object)) {
			Octant octant = root->determineOctant(object);
			if(octant == INVALID_OCTANT) {
				octant = OCTANT0;
			}
			root = root->createEnvolvingNode(octant);
			addObject(object);
		} else {
			root->addObject(object);
		}
	}

	void Octree::queryObjects(QueryList& queryList) {
		root->queryObjects(queryList);
	}

	void Octree::queryObjects(ObjectList& queryList, math::Vector3 center, float maxDistance) {
		root->queryObjects(queryList, center, maxDistance);
	}

	void Octree::removeObject(SceneTreeObject* object) {
		root->removeObject(object);
	}

} // namespace engine

