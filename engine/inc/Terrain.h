/*
 * Terrain.h
 *
 *  Created on: 19/06/2012
 *      Author: marrony
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "math/Vector.h"

namespace engine {

	struct TriangleNode {
		TriangleNode* baseNeighbor;
		TriangleNode* leftNeighbor;
		TriangleNode* rightNeighbor;
		TriangleNode* leftChild;
		TriangleNode* rightChild;
	};

	struct TerrainSection {
	};

	class Terrain {
	public:
		Terrain(int gridX, int gridY, int maxNodes) :
				gridX(gridX), gridY(gridY), maxNodes(maxNodes), nodeCount(0) {
			poolNode = new TriangleNode[maxNodes];
		}

		~Terrain() {
			delete[] poolNode;
		}

		TriangleNode* newNode() {
			TriangleNode* node = 0;

			if(nodeCount < maxNodes) {
				node = &poolNode[nodeCount++];

				node->baseNeighbor = 0;
				node->leftNeighbor = 0;
				node->rightNeighbor = 0;
				node->leftChild = 0;
				node->rightChild = 0;
			}

			return node;
		}

	private:
		float* heightMap;
		math::Vector3* normalMap;
		int gridX;
		int gridY;

		int maxNodes;
		int nodeCount;
		TriangleNode* poolNode;
	};

} /* namespace engine */

#endif /* TERRAIN_H_ */
