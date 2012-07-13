/*
 * Buffer.h
 *
 *  Created on: 22/10/2010
 *      Author: marrony.neris
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <vector>
#include <stddef.h>

namespace engine {

	enum AccessType {
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	struct Buffer {
		virtual ~Buffer() {}

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual int getCount() const = 0;
		virtual void* getPointer() const = 0;
		virtual bool isOk() const = 0;

		virtual void* map(AccessType accessType) = 0;
		virtual void unmap() = 0;
	};

	class MemoryBuffer : public Buffer {
		std::vector<char> buffer;
	public:
		template<typename T>
		MemoryBuffer(const T* data, int elementCount) :
				buffer((char*)data, (char*)(data + elementCount)) {
		}

		MemoryBuffer(int size) :
				buffer(size) {
		}

		virtual ~MemoryBuffer() {
		}

		virtual int getCount() const {
			return buffer.size();
		}

		virtual void* getPointer() const {
			return (void*) buffer.data();
		}

		virtual bool isOk() const {
			return getCount() != 0;
		}

		virtual void bind() {
		}

		virtual void unbind() {
		}

		virtual void* map(AccessType accessType) {
			return getPointer();
		}

		virtual void unmap() {
		}
	};

}  // namespace engine

#endif /* BUFFER_H_ */
