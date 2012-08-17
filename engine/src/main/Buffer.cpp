/*
 * Buffer.cpp
 *
 *  Created on: 06/08/2012
 *      Author: marrony.neris
 */

#include "Buffer.h"
#include "GraphicManager.h"

namespace engine {

	Buffer::Buffer(int size, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess) :
			data(0), handle(0), size(size), bufferType(bufferType),
			frequencyAccess(frequencyAccess), natureAccess(natureAccess),
			manager(0) {
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
		if(manager) {
			data = manager->mapBuffer(this, accessType);
		} else {
			if(!data) data = new char[size];
		}

		dirty = true;

		return data;
	}

	void Buffer::unmap() {
		if(manager) {
			data = manager->unmapBuffer(this);
		}
	}

}  // namespace engine


