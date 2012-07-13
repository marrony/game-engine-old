/*
 * ObjectList.h
 *
 *  Created on: 10/03/2011
 *      Author: marrony
 */

#ifndef OBJECTLIST_H_
#define OBJECTLIST_H_

#include "Bounding.h"

#include <vector>

namespace engine {

	class Frustum;

	enum SceneTreeType {
		GeometrySceneType,
		LightSceneType
	};

	struct SceneTreeObject {
		virtual ~SceneTreeObject() {}

		virtual AABoundingBox getBoundingBox() const = 0;
		virtual bool isVisible(const Frustum* const frustum) const = 0;
		virtual void notify() = 0;

		virtual SceneTreeType getSceneTreeType() const = 0;
	};

	class ObjectList {
	public:
		ObjectList();
		~ObjectList();

		bool isEmpty() const;
		size_t size() const;

		bool contains(SceneTreeObject* object) const;
		void addAll(const ObjectList& other);
		void addObject(SceneTreeObject* object);
		void removeObject(SceneTreeObject* object);
		SceneTreeObject* getObject(size_t index) const;

		void clear();
	private:
		typedef std::vector<SceneTreeObject*> Vector;

		Vector objects;
	};

}

#endif /* OBJECTLIST_H_ */
