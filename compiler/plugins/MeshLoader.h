/*
 * MeshLoader.h
 *
 *  Created on: 03/02/2011
 *      Author: marrony
 */

#ifndef MESHLOADER_H_
#define MESHLOADER_H_

#include "Mesh.h"

#include <istream>
#include <vector>

namespace engine {

	namespace mesh {

		class MeshLoader {
		public:
			virtual ~MeshLoader() { }
			virtual std::vector<Mesh*> loadMesh(std::istream& meshStream) = 0;
		};

	}  // namespace mesh

}  // namespace engine

#endif /* MESHLOADER_H_ */
