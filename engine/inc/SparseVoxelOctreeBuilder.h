/*
 * SVOBuilder.h
 *
 *  Created on: 24/08/2011
 *      Author: marrony
 */

#ifndef SVOBUILDER_H_
#define SVOBUILDER_H_

#include <vector>

#include "math/Vector.h"
#include "Model.h"
#include "GeometryData.h"

namespace engine {

	class Model;
	class SparseVoxelOctree;
	class VoxelOctree;
	class VoxelOctreeNode;

	class SparseVoxelOctreeBuilder {
		Model* mesh;
		std::vector<unsigned short> indices;
		MeshVertex triVertex[3];
	public:
		SparseVoxelOctreeBuilder(Model* mesh);

		SparseVoxelOctree* build(int maxLevel);

		SparseVoxelOctree* buildDebug(int maxLevel, math::Vector3 v0, math::Vector3 v1, math::Vector3 v2);

	private:
		math::Vector3 calculateCenter(int x, int y, int z, int level, float parentDimension, math::Vector3 octreeMin, float& childDimension);

		void traverse(int position, int x, int y, int z,
            math::Vector3 octreeMin, float dimension, math::Vector3 triMin, math::Vector3 triMax,
            int level, int maxLevel, VoxelOctreeNode* parent);

		bool intersects(int x, int y, int z, int level, float dimension,
            math::Vector3 octreeMin, math::Vector3 triMin, math::Vector3 triMax);
	};

} /* namespace engine */
#endif /* SVOBUILDER_H_ */
