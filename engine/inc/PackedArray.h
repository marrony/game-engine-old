/*
 * PackedArray.h
 *
 *  Created on: 26/08/2012
 *      Author: marrony
 */

#ifndef PACKEDARRAY_H_
#define PACKEDARRAY_H_

#include <climits>

namespace engine {

	template<typename Object, int NumberOfBits = 16>
	struct PackedArray {
		typedef unsigned int ID;

		struct Index {
			ID id;
			unsigned short index;
			unsigned short next;
		};

		static const unsigned int MAX_OBJECTS = 1 << NumberOfBits;
		static const unsigned int INDEX_MASK = MAX_OBJECTS - 1;
		static const unsigned int NEW_OBJECT_ID_ADD = MAX_OBJECTS;

		Object objects[MAX_OBJECTS];
		Index indices[MAX_OBJECTS];
		unsigned int numberObjects;
		unsigned int freelistEnqueue;
		unsigned int freelistDequeue;

		PackedArray() {
			numberObjects = 0;

			for(unsigned int i = 0; i < MAX_OBJECTS; ++i) {
				indices[i].id = i;
				indices[i].next = i + 1;
			}

			freelistDequeue = 0;
			freelistEnqueue = MAX_OBJECTS - 1;
		}

		inline bool has(ID id) const {
			Index &in = indices[id & INDEX_MASK];
			return in.id == id && in.index != USHRT_MAX;
		}

		inline Object &lookup(ID id) const {
			return objects[indices[id & INDEX_MASK].index];
		}

		inline ID add() {
			Index &in = indices[freelistDequeue];
			freelistDequeue = in.next;
			in.id += NEW_OBJECT_ID_ADD;
			in.index = numberObjects++;
			Object &o = objects[in.index];
			o.id = in.id;
			return o.id;
		}

		inline void remove(ID id) {
			Index &in = indices[id & INDEX_MASK];

			Object &o = objects[in.index];
			o = objects[--numberObjects];
			indices[o.id & INDEX_MASK].index = in.index;

			in.index = USHRT_MAX;
			indices[freelistEnqueue].next = id & INDEX_MASK;
			freelistEnqueue = id & INDEX_MASK;
		}
	};

} // namespace engine

#endif /* PACKEDARRAY_H_ */
