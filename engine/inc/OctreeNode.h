/*
 * OctreeNode.h
 *
 *  Created on: 24/03/2011
 *      Author: marrony.neris
 */

#ifndef OCTREENODE_H_
#define OCTREENODE_H_

#include "math/Vector.h"
#include "QueryList.h"
#include "OctreeConfiguration.h"

namespace engine {

	enum Octant {
		INVALID_OCTANT = -1,
		OCTANT0 = 0,
		OCTANT1,
		OCTANT2,
		OCTANT3,
		OCTANT4,
		OCTANT5,
		OCTANT6,
		OCTANT7,
		MAX_OCTANTS
	};

	class OctreeNode {
	public:
		OctreeNode(const OctreeConfiguration& newConfiguration, OctreeNode* newParent, const math::Vector3& newCenter, float newSize, int newLevel = 0);
		~OctreeNode();

		bool hasChildren() const;
		OctreeNode* getChildAt(Octant index) const;

		void queryObjects(QueryList& queryList);
		void queryObjects(ObjectList& queryList, math::Vector3 center, float maxDistance);

		ObjectList getObjectList() const;

		void addObjects(const ObjectList& objectList);
		void addObject(SceneTreeObject* object);
		void removeObject(SceneTreeObject* object);

		bool isInside(SceneTreeObject* object) const;
		AABoundingBox getBoundingBox() const;

		OctreeNode* createEnvolvingNode(Octant octant);

		Octant determineOctant(SceneTreeObject* object);
	private:
		void setChildAt(OctreeNode* child, Octant index);

		void splitObjects(ObjectList childObjects[MAX_OCTANTS]);

		size_t objectsCount();
		void mergeChilds();

		void tryToMerge();
		void tryToSplit();

		void updateLevel();

		OctreeNode* createChild(Octant childNumber);

		OctreeNode* parent;
		OctreeNode* childs[MAX_OCTANTS];
		math::Vector3 center;
		int level;
		float size;
		ObjectList objectList;
		const OctreeConfiguration& configuration;

		static const math::Vector3 centers[MAX_OCTANTS];
		static Octant determineOctant(const math::Vector3& pt, const math::Vector3& center);
	};
}

#endif /* OCTREENODE_H_ */
