/*
 * SceneTreeBuilder.h
 *
 *  Created on: 11/03/2011
 *      Author: marrony.neris
 */

#ifndef SCENETREEBUILDER_H_
#define SCENETREEBUILDER_H_

#include "SceneTree.h"
#include "ObjectList.h"

namespace engine {

	class SceneTreeBuilder {
	public:
		virtual ~SceneTreeBuilder() { }

		virtual SceneTree* build(const ObjectList& objectList) = 0;
	};

}

#endif /* SCENETREEBUILDER_H_ */
