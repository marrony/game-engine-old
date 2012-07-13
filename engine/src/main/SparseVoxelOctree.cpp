/*
 * SVOctree.cpp
 *
 *  Created on: 24/08/2011
 *      Author: marrony
 */

#include "SparseVoxelOctree.h"

#include "Frustum.h"

#include <limits>
#include <iostream>

namespace engine {

	math::Vector3 SparseVoxelOctree::calculateCenter(int x, int y, int z, int level, float parentDimension, math::Vector3 octreeMin, float& childDimension) {
		childDimension = parentDimension / (1 << level);
		float halfDimension = childDimension * 0.5;

		math::Vector3 center(x, y, z);
		center *= childDimension;
		center += halfDimension;
		center += octreeMin;

		return center;
	}

	void SparseVoxelOctree::render(Frustum* frustum, std::vector<math::Vector3>& colorBuffer, std::vector<float>& depthBuffer, size_t width, size_t height) {
		float dim = -0.5 * dimension;
		math::Vector3 octreeMin(dim, dim, dim);

		traverse(0, 0, 0, 0, frustum, 0, dimension * 0.5, octreeMin, colorBuffer, depthBuffer, width, height);
	}

	void SparseVoxelOctree::traverse(size_t offset, int x, int y, int z, Frustum* frustum, int level, float halfDim, math::Vector3 octreeMin, std::vector<math::Vector3>& colorBuffer, std::vector<float>& depthBuffer, size_t width, size_t height) {
		float pixelWidth = 1.0 / width;
		float pixelHeight = 1.0 / height;

		float childDimension;
		bool subdivide = false;

		math::Vector3 center = calculateCenter(x, y, z, level, dimension, octreeMin, childDimension);

		VisualData visualData = offset < innerNodes.size() ?
				innerNodes[offset]->visualData :
				leaves[offset - innerNodes.size()]->visualData;

		if(offset >= innerNodes.size()) {
			center = frustum->getViewProjectionMatrix() | center;
			halfDim *= 0.5 * childDimension;
		} else {
			center = frustum->getViewMatrix() | center;
			math::Vector3 dimVec(0.5 * childDimension, 0.5 * childDimension, center.z);
			center = frustum->getProjectionMatrix() | center;
			dimVec = frustum->getProjectionMatrix() | dimVec;
			halfDim = dimVec.x;
			subdivide = halfDim > pixelWidth;
		}

		if(subdivide) {
			float minusOneMinusHalfDim = -1.0 - halfDim;
			float onePlusHalfDim = 1.0 + halfDim;

			if(center.x < minusOneMinusHalfDim || center.x > onePlusHalfDim) return;
			if(center.y < minusOneMinusHalfDim || center.y > onePlusHalfDim) return;
			if(center.z < minusOneMinusHalfDim + 1.0f || center.z > onePlusHalfDim) return;

			int childIndex = 0;
			size_t mask = innerNodes[offset]->mask;
			size_t index = innerNodes[offset]->childIndex;

			if(mask == 0) {
				return;
			}

			for(int i = 0; i < 8; i++) {
				if((mask & (1 << i)) != 0) {
					traverse(index + childIndex,
							x * 2 + ((i & 1) >> 0),
							y * 2 + ((i & 2) >> 1),
							z * 2 + ((i & 4) >> 2),
							frustum,
							level + 1,
							halfDim,
							octreeMin,
							colorBuffer,
							depthBuffer,
							width,
							height);
					childIndex++;
				}
			}
		} else {
			center.x = center.x * 0.5 + 0.5;
			center.y = center.y * 0.5 + 0.5;

			int centerX = (int)round(center.x * width);
			int centerY = (int)round(center.y * height);

			if(centerX < 0 || centerX >= width) return;
			if(centerY < 0 || centerY >= height) return;

			size_t index = centerY * width + centerX;

			if(center.z < depthBuffer[index]) {
				float color = std::min(std::max(0.0f, math::dot(visualData.normal, math::Vector3(0, 1, 0))), 1.0f);

				colorBuffer[index] = math::Vector3(color, color, color);
				depthBuffer[index] = center.z;
			}
		}
	}

} /* namespace engine */
