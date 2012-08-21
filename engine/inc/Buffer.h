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

	class Buffer {
		int handle;
		char* data;
		int size;
		BufferType bufferType;
		FrequencyAccess frequencyAccess;
		NatureAccess natureAccess;
		bool dirty;
	public:
		Buffer(int size, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess);
		~Buffer();

		int getHandle() const;
		void setHandle(int handle);

		int getSize() const;
		BufferType getBufferType() const;
		FrequencyAccess getFrequencyAccess() const;
		NatureAccess getNatureAccess() const;
		const void* getData() const;

		bool needUpdate() const;
		void setUpdated();
	};

}  // namespace engine

#endif /* BUFFER_H_ */
