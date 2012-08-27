/*
 * RenderingSystem.h
 *
 *  Created on: 27/08/2012
 *      Author: marrony.neris
 */

#ifndef RENDERINGSYSTEM_H_
#define RENDERINGSYSTEM_H_

#include "math/Matrix.h"

#include <vector>

namespace engine {

	class RenderingSystem {
		std::vector<math::Matrix4> global;
		std::vector<math::Matrix4> local;
		std::vector<unsigned int> parents;
		std::vector<unsigned int> childs;

		typedef unsigned int ID;

		struct Index {
			ID id;
			unsigned int free : 1;
			unsigned int index : 16;
		};

		unsigned short freeList;
		int numObjects;
		Index indices[1024];
		int objects[1024];
	public:
		RenderingSystem() {
			global.push_back(math::Matrix4::IDENTITY);
			local.push_back(math::Matrix4::IDENTITY);
			parents.push_back(0);
			childs.push_back(0);

			for(int i = 0; i < 1024; ++i) {
				indices[i].free = 1;
				indices[i].index = i + 1;
			}

			numObjects = 1;
			freeList = 1;
			indices[0].free = 0;
			indices[0].index = 0;
		}

		ID insertNode(ID parent, math::Matrix4 local) {
			Index& c = indices[freeList];

			freeList = c.index;

			Index& p = indices[parent & 0xffff];
			childs[p.index]++;

			c.free = 0;
			c.index = p.index + childs[p.index];

			numObjects++;
			childs.resize(std::max((size_t)numObjects, childs.size()));
			parents.resize(std::max((size_t)numObjects, parents.size()));

			for(int i = numObjects - 1; i > c.index; --i) {
				parents[i] = parents[i - 1];
				childs[i] = childs[i - 1];
			}

			parents[c.index] = p.index;

			Index* i = &indices[0];
			while(i < indices+1024) {
				if(i != &c && !i->free && i->index >= c.index)
					i->index++;

				i++;
			}
		}
	};

} /* namespace engine */

#endif /* RENDERINGSYSTEM_H_ */
