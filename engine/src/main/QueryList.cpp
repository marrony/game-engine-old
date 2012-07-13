/*
 * QueryList.cpp
 *
 *  Created on: 14/03/2011
 *      Author: marrony
 */

#include "QueryList.h"

namespace engine {
	QueryList::QueryList() :
			frustum(0), objectList(), filter() {
	}

	void QueryList::addObject(SceneTreeObject* object) {
		objectList.addObject(object);
	}

	SceneTreeObject* QueryList::getObject(size_t index) const {
		return objectList.getObject(index);
	}

	void QueryList::setFrustum(const Frustum *newFrustum) {
		frustum = newFrustum;
	}

	const Frustum* QueryList::getFrustum() const {
		return frustum;
	}

	void QueryList::setFilter(SceneTreeType newFilter) {
		filter = newFilter;
	}

	SceneTreeType QueryList::getFilter() const {
		return filter;
	}

	bool QueryList::isEmpty() const {
		return objectList.isEmpty();
	}

	size_t QueryList::size() const {
		return objectList.size();
	}

// namespace engine
}

