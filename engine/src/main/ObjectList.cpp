/*
 * ObjectList.cpp
 *
 *  Created on: 10/03/2011
 *      Author: marrony
 */

#include "ObjectList.h"
#include <algorithm>

namespace engine {

	ObjectList::ObjectList() {
	}

	ObjectList::~ObjectList() {
	}

	bool ObjectList::isEmpty() const {
		return objects.empty();
	}

	size_t ObjectList::size() const {
		return objects.size();
	}

	bool ObjectList::contains(SceneTreeObject* object) const {
		return std::find(objects.begin(), objects.end(), object) != objects.end();
	}

	void ObjectList::addAll(const ObjectList& other) {
		for(int i = 0; i < other.size(); i++) {
			addObject(other.getObject(i));
		}
	}

	void ObjectList::addObject(SceneTreeObject* object) {
		objects.push_back(object);
	}

	void  ObjectList::removeObject(SceneTreeObject* object) {
		Vector::iterator ite = std::remove(objects.begin(), objects.end(), object);
		objects.erase(ite, objects.end());
	}

	SceneTreeObject* ObjectList::getObject(size_t index) const {
		return objects.at(index);
	}

	void ObjectList::clear() {
		objects.clear();
	}
}
