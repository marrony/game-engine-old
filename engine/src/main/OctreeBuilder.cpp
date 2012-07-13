/*
 * OctreeBuilder.cpp
 *
 *  Created on: 11/03/2011
 *      Author: marrony.neris
 */

#include "OctreeBuilder.h"
#include "Octree.h"

namespace engine {

	OctreeBuilder::OctreeBuilder() {
	}

	OctreeBuilder::~OctreeBuilder() {
	}

	SceneTree* OctreeBuilder::build(const ObjectList& objectList) {

		for(size_t i = 0; i < objectList.size(); i++) {
			SceneTreeObject* object = objectList.getObject(i);

			AABoundingBox bounding = object->getBoundingBox();
		}

		return 0;
	}
}
