/*
 * SVOctree.h
 *
 *  Created on: 24/08/2011
 *      Author: marrony
 */

#ifndef SVOCTREE_H_
#define SVOCTREE_H_

#include "math/Vector.h"

#include <vector>

namespace engine {

	class Frustum;

	struct VisualData {
		math::Vector3 normal;
		math::Vector2 texCoord;
	};

	struct SparseVoxelInnerNode {
		size_t mask;
		size_t childIndex;
		VisualData visualData;

		SparseVoxelInnerNode(size_t mask, size_t childIndex, const VisualData& visualData) :
				mask(mask), childIndex(childIndex), visualData(visualData) {
		}
	};

	struct SparseVoxelLeaf {
		VisualData visualData;

		SparseVoxelLeaf(const VisualData& visualData) :
				visualData(visualData) {
		}
	};

	class SparseVoxelOctree {
		std::vector<SparseVoxelInnerNode*> innerNodes;
		std::vector<SparseVoxelLeaf*> leaves;
		float dimension;

		math::Vector3 calculateCenter(int x, int y, int z, int level, float parentDimension, math::Vector3 octreeMin, float& childDimension);

		void traverse(size_t offset, int x, int y, int z, Frustum* frustum, int level, float halfDim, math::Vector3 octreeMin, std::vector<math::Vector3>& colorBuffer, std::vector<float>& depthBuffer, size_t width, size_t height);
	public:
		SparseVoxelOctree(std::vector<SparseVoxelInnerNode*> innerNodes, std::vector<SparseVoxelLeaf*> leaves, float dimension) :
				innerNodes(innerNodes), leaves(leaves), dimension(dimension) {
		}

		void render(Frustum* frustum, std::vector<math::Vector3>& colorBuffer, std::vector<float>& depthBuffer, size_t width, size_t height);
	};

} /* namespace engine */
#endif /* SVOCTREE_H_ */
