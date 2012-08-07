/*
 * Buffer.cpp
 *
 *  Created on: 06/08/2012
 *      Author: marrony.neris
 */

#include "Buffer.h"

namespace engine {

	Buffer::Buffer(int size, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess) :
			data(0), handle(0), size(size), bufferType(bufferType),
			frequencyAccess(frequencyAccess), natureAccess(natureAccess) {
	}

	Buffer::~Buffer() {
	}

	int Buffer::getHandle() const {
		return handle;
	}

	void Buffer::setHandle(int handle) {
		this->handle = handle;
	}

	int Buffer::getSize() const {
		return size;
	}

	BufferType Buffer::getBufferType() const {
		return bufferType;
	}

	FrequencyAccess Buffer::getFrequencyAccess() const {
		return frequencyAccess;
	}

	NatureAccess Buffer::getNatureAccess() const {
		return natureAccess;
	}

	const void* Buffer::getData() const {
		return data;
	}

	bool Buffer::needUpdate() const {
		return dirty;
	}

	void Buffer::setUpdated() {
		dirty = false;

		if(data) delete[] data;
		data = nullptr;
	}

	void* Buffer::map(AccessType accessType) {
		if(!data) data = new char[size];

		dirty = true;

		return data;
	}

	void Buffer::unmap() {
	}

}  // namespace engine


