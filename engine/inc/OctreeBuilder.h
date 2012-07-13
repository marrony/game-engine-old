/*
 * OctreeBuilder.h
 *
 *  Created on: 11/03/2011
 *      Author: marrony.neris
 */

#ifndef OCTREEBUILDER_H_
#define OCTREEBUILDER_H_

#include "SceneTreeBuilder.h"

namespace engine {

	class OctreeBuilder : public SceneTreeBuilder {
	public:
		OctreeBuilder();
		virtual ~OctreeBuilder();

		virtual SceneTree* build(const ObjectList& objectList);
	};

}

#endif /* OCTREEBUILDER_H_ */
