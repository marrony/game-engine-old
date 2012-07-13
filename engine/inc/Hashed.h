/*
 * Hashed.h
 *
 *  Created on: 12/04/2012
 *      Author: marrony.neris
 */

#ifndef HASHED_H_
#define HASHED_H_

namespace engine {

	class Hashed {
	public:
		Hashed(const char* typeName);

		bool operator<(const Hashed& other) const;

		bool operator==(const Hashed& other) const;
	private:
		union {
			const void* hexCode;
			unsigned int hashCode;
		};
	};

	inline Hashed::Hashed(const char* typeName) :
			hashCode(JSHash(typeName, strlen(typeName))) {
	}

	inline bool Hashed::operator<(const Hashed& other) const {
		return hashCode < other.hashCode;
	}

	inline bool Hashed::operator==(const Hashed& other) const {
		return hashCode == other.hashCode;
	}

}  // namespace engine


#endif /* HASHED_H_ */
