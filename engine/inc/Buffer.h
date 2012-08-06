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

#include "DeviceObject.h"

namespace engine {

	enum class AccessType : unsigned char {
		ReadOnly, WriteOnly, ReadWrite
	};

	enum class FrequencyAccess : unsigned char {
		Stream, Static, Dynamic
	};

	enum class NatureAccess : unsigned char {
		Draw, Read, Copy
	};

	enum class BufferType : unsigned char {
		VertexBuffer, IndexBuffer
	};

	class Buffer : public DeviceObject {
		int handle;
		void* data;
		int size;
		BufferType bufferType;
		FrequencyAccess frequencyAccess;
		NatureAccess natureAccess;
	public:
		Buffer(int size, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess);
		virtual ~Buffer();

		virtual int getHandle() const;
		virtual void setHandle(int handle);

		int getSize() const { return size; }
		BufferType getBufferType() const { return bufferType; }
		FrequencyAccess getFrequencyAccess() const { return frequencyAccess; }
		NatureAccess getNatureAccess() const { return natureAccess; }

		void* map(AccessType accessType);
		void unmap();
	};

}  // namespace engine

#endif /* BUFFER_H_ */
