/*
 * EventType.h
 *
 *  Created on: 31/10/2011
 *      Author: marrony
 */

#ifndef TYPE_H_
#define TYPE_H_

#include <string.h>

#include "GeneralHashFunction.h"

namespace engine {

	class Type {
	public:
		Type(const char* typeName = 0);

		bool operator<(const Type& other) const;

		bool operator==(const Type& other) const;

		const char* getName() const;
	private:
		void calculateHash();

		union {
			const void* hexCode;
			unsigned int hashCode;
		};
		const char* typeName;
	};

	inline Type::Type(const char* typeName) :
			hashCode(), typeName(typeName) {
		calculateHash();
	}

	inline bool Type::operator<(const Type& other) const {
		return hashCode < other.hashCode;
	}

	inline bool Type::operator==(const Type& other) const {
		return hashCode == other.hashCode;
	}

	inline const char* Type::getName() const {
		return typeName;
	}

	inline void Type::calculateHash() {
		if(typeName)
			hashCode = JSHash(typeName, strlen(typeName));
	}

} /* namespace engine */

#endif /* TYPE_H_ */
