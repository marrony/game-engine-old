/*
 * SVOBuilder.cpp
 *
 *  Created on: 24/08/2011
 *      Author: marrony
 */

#include "SparseVoxelOctreeBuilder.h"
#include "SparseVoxelOctree.h"
#include "Bounding.h"
#include "math/Vector.h"

#include <limits>
#include <stack>
#include <queue>

namespace engine {

	class VoxelOctreeNode {
	public:
		float dist;
		int position;
		VisualData visualData;
		VoxelOctreeNode* childs[8];

		VoxelOctreeNode(int position) :
				dist(std::numeric_limits<float>::max()), position(position) {
			memset(childs, 0, sizeof(childs));
		}

		VoxelOctreeNode* addChild(int position) {
			if(!childs[position]) {
				childs[position] = new VoxelOctreeNode(position);
			}

			return childs[position];
		}

		size_t getChildCount() const {
			size_t count = 0;

			for(int i = 0; i < 8; i++) {
				count += (childs[i] != 0);
			}

			return count;
		}

		size_t getLeafCount() const {
			size_t count = (getChildCount() == 0);

			for(int i = 0; i < 8; i++) {
				if(childs[i]) {
					count += childs[i]->getLeafCount();
				}
			}

			return count;
		}

		size_t getNodeCount() const {
			size_t count = 1;

			for(int i = 0; i < 8; i++) {
				if(childs[i]) {
					count += childs[i]->getNodeCount();
				}
			}

			return count;
		}
	};

	class VoxelOctree : public VoxelOctreeNode {
		float dimension;
	public:
		VoxelOctree(float dimension) :
				VoxelOctreeNode(0), dimension(dimension) {
		}

		SparseVoxelOctree* convert() {
			size_t leafCount = getLeafCount();
			size_t innerNodeCount = getNodeCount() - leafCount;

			std::vector<SparseVoxelInnerNode*> innerNodes(innerNodeCount);
			std::vector<SparseVoxelLeaf*> leafs(leafCount);

			std::queue<VoxelOctreeNode*> stack;
			stack.push(this);

			size_t offset = 1;
			size_t index = 0;

			while(!stack.empty()) {
				VoxelOctreeNode* node = stack.front();
				stack.pop();

				size_t mask = 0;
				if(node->getChildCount() > 0) {
					for(int i = 0; i < 8; i++) {
						VoxelOctreeNode* child = node->childs[i];

						if(child) {
							stack.push(child);

							mask |= (1 << child->position);
						}
					}
				}

				if(index < innerNodeCount) {
					innerNodes[index++] = new SparseVoxelInnerNode(mask, offset, node->visualData);
				} else {
					leafs[index++ - innerNodeCount] = new SparseVoxelLeaf(node->visualData);
				}

				offset += node->getChildCount();
			}

			return new SparseVoxelOctree(innerNodes, leafs, dimension);
		}
	};

	SparseVoxelOctreeBuilder::SparseVoxelOctreeBuilder(Model* mesh) :
			mesh(mesh), indices() {
		//indices = mesh->getIndices();
	}

	SparseVoxelOctree* SparseVoxelOctreeBuilder::build(int maxLevel) {
		AABoundingBox bbox;// = mesh->getBoundingBox();
		math::Vector3 diff = bbox.getMax() - bbox.getMin();
		float maxDim = std::max(diff.x, std::max(diff.y, diff.z));
		float halfDim = maxDim * 0.5;
		math::Vector3 center = diff * 0.5 + bbox.getMin();
		math::Vector3 octMin = center - math::Vector3(halfDim, halfDim, halfDim);
		math::Vector3 octMax = center + math::Vector3(halfDim, halfDim, halfDim);

		VoxelOctree* voxelOctree = new VoxelOctree(maxDim);

		for(size_t triangle = 0; triangle < indices.size(); triangle += 3) {
			math::Vector3 triMin = math::Vector3::max();
			math::Vector3 triMax = math::Vector3::min();

			for(size_t j = 0; j < 3; j++) {
				//triVertex[j] = mesh->getVertex(indices[triangle + j]);

				math::Vector3 pos = triVertex[j].position;

				triMin = math::min(triMin, pos);
				triMax = math::max(triMax, pos);
			}

			int position = 0;
			int x = 0;
			int y = 0;
			int z = 0;
			int level = 0;

			traverse(position, x, y, z, octMin, maxDim, triMin, triMax, level, maxLevel, voxelOctree);
		}

		return voxelOctree->convert();
	}

	SparseVoxelOctree* SparseVoxelOctreeBuilder::buildDebug(int maxLevel, math::Vector3 v0, math::Vector3 v1, math::Vector3 v2) {
		math::Vector3 e0 = v1 - v0;
		math::Vector3 e1 = v2 - v0;
		math::Vector3 normal = math::cross(e0, e1).normalize();
		triVertex[0].position = v0;
		triVertex[0].normal = normal;

		triVertex[1].position = v1;
		triVertex[1].normal = normal;

		triVertex[2].position = v2;
		triVertex[2].normal = normal;

		math::Vector3 triMin = math::Vector3::max();
		math::Vector3 triMax = math::Vector3::min();

		for(size_t j = 0; j < 3; j++) {
			math::Vector3 pos = triVertex[j].position;

			triMin = math::min(triMin, pos);
			triMax = math::max(triMax, pos);
		}

		math::Vector3 diff = triMax - triMin;
		float maxDim = std::max(diff.x, std::max(diff.y, diff.z));
		float halfDim = maxDim * 0.5;
		math::Vector3 center = diff * 0.5 + triMin;
		math::Vector3 octMin = center - math::Vector3(halfDim, halfDim, halfDim);
		math::Vector3 octMax = center + math::Vector3(halfDim, halfDim, halfDim);

		VoxelOctree* voxelOctree = new VoxelOctree(maxDim);
		int position = 0;
		int x = 0;
		int y = 0;
		int z = 0;
		int level = 0;
		traverse(position, x, y, z, octMin, maxDim, triMin, triMax, level, maxLevel, voxelOctree);

		return voxelOctree->convert();
	}

	math::Vector3 SparseVoxelOctreeBuilder::calculateCenter(int x, int y, int z, int level, float parentDimension, math::Vector3 octreeMin, float& childDimension) {
		childDimension = parentDimension / (1 << level);
		float halfDimension = childDimension * 0.5;

		math::Vector3 center(x, y, z);
		center *= childDimension;
		center += halfDimension;
		center += octreeMin;

		return center;
	}

	void SparseVoxelOctreeBuilder::traverse(int position, int x, int y, int z, math::Vector3 octreeMin, float dimension,
			math::Vector3 triMin, math::Vector3 triMax, int level, int maxLevel, VoxelOctreeNode* parent) {
		if(level == 0) {
			for(int i = 0; i < 8; i++) {
				int newX = x * 2 + ((i & 1) >> 0);
				int newY = y * 2 + ((i & 2) >> 1);
				int newZ = z * 2 + ((i & 4) >> 2);

				traverse(i, newX, newY, newZ, octreeMin, dimension, triMin, triMax, level + 1, maxLevel, parent);
			}
		} else {
			if(intersects(x, y, z, level, dimension, octreeMin,  triMin, triMax)) {
				VoxelOctreeNode* newParent = parent->addChild(position);

				math::Vector3 center;
				float cellDimension;

				center = calculateCenter(x, y, z, level, dimension, octreeMin, cellDimension);

				float triArea = math::cross(triVertex[1].position - triVertex[0].position, triVertex[2].position - triVertex[0].position).length() * 0.5;
				float u = math::cross(triVertex[1].position - center, triVertex[2].position - center).length() * 0.5;
				float v = math::cross(triVertex[0].position - center, triVertex[2].position - center).length() * 0.5;
				float w = math::cross(triVertex[0].position - center, triVertex[1].position - center).length() * 0.5;

				float sum = u + v + w;

				if(sum < newParent->dist) {
					newParent->dist = sum;

					triArea = 1.0 / triArea;
					u *= triArea;
					v *= triArea;
					w *= triArea;

					sum = 1.0 / (u + v + w);
					u *= sum;
					v *= sum;
					w *= sum;

					newParent->visualData.normal = (u * triVertex[0].normal + v * triVertex[1].normal + w * triVertex[2].normal).normalize();
					newParent->visualData.texCoord = u * triVertex[0].texCoord + v * triVertex[1].texCoord + w * triVertex[2].texCoord;
				}

				if(level < maxLevel) {
					for(int i = 0; i < 8; i++) {
						int newX = x * 2 + ((i & 1) >> 0);
						int newY = y * 2 + ((i & 2) >> 1);
						int newZ = z * 2 + ((i & 4) >> 2);

						traverse(i, newX, newY, newZ, octreeMin, dimension, triMin, triMax, level + 1, maxLevel, newParent);
					}
				}
			}
		}
	}

	bool triBoxOverlap(math::Vector3 boxcenter, math::Vector3 boxhalfsize, math::Vector3 triverts[3]);

	bool SparseVoxelOctreeBuilder::intersects(int x, int y, int z, int level, float dimension, math::Vector3 octreeMin,
			math::Vector3 triMin, math::Vector3 triMax) {

      math::Vector3 center;
      float childDimension;

      center = calculateCenter(x, y, z, level, dimension, octreeMin, childDimension);

      math::Vector3 triverts[3];

      triverts[0] = triVertex[0].position;;
      triverts[1] = triVertex[1].position;
      triverts[2] = triVertex[2].position;

		return triBoxOverlap(center.vector, math::Vector3(childDimension, childDimension, childDimension).vector, triverts);
	}

	bool planeBoxOverlap(math::Vector3 normal, math::Vector3 vert, math::Vector3 maxbox) {
		int q;
		math::Vector3 vmin, vmax;
		float v;

		for(q = 0; q < 3; q++) {
			v = vert.vector[q];

			if(normal.vector[q] > 0.0f) {
				vmin.vector[q] = -maxbox.vector[q] - v;
				vmax.vector[q] = +maxbox.vector[q] - v;
			} else {
				vmin.vector[q] = +maxbox.vector[q] - v;
				vmax.vector[q] = -maxbox.vector[q] - v;
			}
		}

		if(math::dot(normal, vmin) > 0.0f)
			return false;

		if(math::dot(normal, vmax) >= 0.0f)
			return true;

		return false;
	}

	void findMinMax(float x0, float x1, float x2, float& min, float& max) {
		min = max = x0;
		if(x1 < min) min = x1;
		if(x1 > max) max = x1;
		if(x2 < min) min = x2;
		if(x2 > max) max = x2;
	}

	bool triBoxOverlap(math::Vector3 boxcenter, math::Vector3 boxhalfsize, math::Vector3 triverts[3])
	{
		/*    use separating axis theorem to test overlap between triangle and box */
		/*    need to test for overlap in these directions: */
		/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
		/*       we do not even need to test these) */
		/*    2) normal of the triangle */
		/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
		/*       this gives 3x3=9 more tests */

		float min, max;

		/* This is the fastest branch on Sun */
		/* move everything so that the boxcenter is in (0,0,0) */
		math::Vector3 v0 = triverts[0] - boxcenter;
		math::Vector3 v1 = triverts[1] - boxcenter;
		math::Vector3 v2 = triverts[2] - boxcenter;

		/* compute triangle edges */
		math::Vector3 edges[3] = {
				v1 - v0,
				v2 - v1,
				v0 - v2
		};

		math::Vector3 axis[] = {
				math::Vector3(1, 0, 0),
				math::Vector3(0, 1, 0),
				math::Vector3(0, 0, 1)
		};

		/* Bullet 3:  */
		/*  test the 9 tests first (this was faster) */
		for(int ax = 0; ax < 3; ax++) {
			for(int ed = 0; ed < 3; ed++) {
				math::Vector3 a = math::cross(axis[ax], edges[ed]);

				float p0 = math::dot(a, v0);
				float p1 = math::dot(a, v1);
				float p2 = math::dot(a, v2);

				min = std::min(p0, std::min(p1, p2));
				max = std::max(p0, std::max(p1, p2));

				a = math::abs(a);

				float rad = math::dot(a, boxhalfsize);

				if(min > rad || max < -rad)
					return false;
			}
		}

		/* Bullet 1: */
		/*  first test overlap in the {x,y,z}-directions */
		/*  find min, max of the triangle each direction, and test for overlap in */
		/*  that direction -- this is equivalent to testing a minimal AABB around */
		/*  the triangle against the AABB */
		/* test in X-direction */

		findMinMax(v0.x, v1.x, v2.x, min, max);
		if(min > boxhalfsize.x || max < -boxhalfsize.x)
			return false;

		/* test in Y-direction */
		findMinMax(v0.y, v1.y, v2.y, min, max);
		if(min > boxhalfsize.y || max < -boxhalfsize.y)
			return false;

		/* test in Z-direction */
		findMinMax(v0.z, v1.z, v2.z, min, max);
		if(min > boxhalfsize.z || max < -boxhalfsize.z)
			return false;

		/* Bullet 2: */
		/*  test if the box intersects the plane of the triangle */
		/*  compute plane equation of triangle: normal*x+d=0 */
		math::Vector3 normal = math::cross(edges[0], edges[1]);

		return planeBoxOverlap(normal, v0, boxhalfsize);
	}


} /* namespace engine */
