/*
 * QueryList.h
 *
 *  Created on: 14/03/2011
 *      Author: marrony
 */

#ifndef QUERYLIST_H_
#define QUERYLIST_H_

#include "ObjectList.h"

namespace engine {

	class Frustum;

	class QueryList {
	public:
		QueryList();

		void addObject(SceneTreeObject* object);
		SceneTreeObject* getObject(size_t index) const;

		void setFrustum(const Frustum* newFrustum);
		const Frustum* getFrustum() const;

		void setFilter(SceneTreeType filter);
		SceneTreeType getFilter() const;

		bool isEmpty() const;
		size_t size() const;
	private:
		const Frustum* frustum;
		ObjectList objectList;
		SceneTreeType filter;
	};

}

#endif /* QUERYLIST_H_ */
