/*
 * HardwareBuffer.cpp
 *
 *  Created on: 15/06/2012
 *      Author: marrony.neris
 */

#include "HardwareBuffer.h"
#include "System.h"

#include "MemoryManager.h"

namespace engine {

	void HardwareBuffer::ensureBuffer() {
		if(id) return;

		glGenBuffers(1, &id);

		glBindBuffer(target, id);
		glBufferData(target, count, 0, usage);
	}

	void HardwareBuffer::destroyBuffer() {
		glDeleteBuffers(1, &id);
	}

	int HardwareBuffer::getTarget(BufferType type) {
		switch(type) {
		case BufferType::VertexBuffer:
			return GL_ARRAY_BUFFER;

		case BufferType::IndexBuffer:
			return GL_ELEMENT_ARRAY_BUFFER;
		}

		return 0;
	}

	int HardwareBuffer::getUsage(FrequencyAccess frequencyAccess, NatureAccess natureAccess) {
		if(frequencyAccess == FrequencyAccess::Stream && natureAccess == NatureAccess::Draw)
			return GL_STREAM_DRAW;

		if(frequencyAccess == FrequencyAccess::Stream && natureAccess == NatureAccess::Read)
			return GL_STREAM_READ;

		if(frequencyAccess == FrequencyAccess::Stream && natureAccess == NatureAccess::Copy)
			return GL_STREAM_COPY;

		if(frequencyAccess == FrequencyAccess::Static && natureAccess == NatureAccess::Draw)
			return GL_STATIC_DRAW;

		if(frequencyAccess == FrequencyAccess::Static && natureAccess == NatureAccess::Read)
			return GL_STATIC_READ;

		if(frequencyAccess == FrequencyAccess::Static && natureAccess == NatureAccess::Copy)
			return GL_STATIC_COPY;

		if(frequencyAccess == FrequencyAccess::Dynamic && natureAccess == NatureAccess::Draw)
			return GL_DYNAMIC_DRAW;

		if(frequencyAccess == FrequencyAccess::Dynamic && natureAccess == NatureAccess::Read)
			return GL_DYNAMIC_READ;

		if(frequencyAccess == FrequencyAccess::Dynamic && natureAccess == NatureAccess::Copy)
			return GL_DYNAMIC_COPY;

		return 0;
	}

	int HardwareBuffer::getAccess(AccessType accessType) {
		if(accessType == AccessType::ReadOnly)
			return GL_READ_ONLY;

		if(accessType == AccessType::WriteOnly)
			return GL_WRITE_ONLY;

		if(accessType == AccessType::ReadWrite)
			return GL_READ_WRITE;

		return 0;
	}

	HardwareBuffer::HardwareBuffer(int count, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess) :
			id(0), count(count), target(getTarget(bufferType)), usage(getUsage(frequencyAccess, natureAccess)) {
	}

	HardwareBuffer::~HardwareBuffer() {
		destroyBuffer();
	}

	void HardwareBuffer::bind() {
		ensureBuffer();

		glBindBuffer(target, id);
	}

	void HardwareBuffer::unbind() {
		glBindBuffer(target, 0);
	}

	int HardwareBuffer::getCount() const {
		return count;
	}

	void* HardwareBuffer::getPointer() const {
		return 0;
	}

	bool HardwareBuffer::isOk() const {
		return true;
	}

	void* HardwareBuffer::map(AccessType accessType) {
		ensureBuffer();

		return glMapBuffer(target, getAccess(accessType));
	}

	void HardwareBuffer::unmap() {
		glUnmapBuffer(target);
	}

} /* namespace engine */
