/*
 * HardwareBuffer.h
 *
 *  Created on: 15/06/2012
 *      Author: marrony.neris
 */

#ifndef HARDWAREBUFFER_H_
#define HARDWAREBUFFER_H_

#include "Buffer.h"

namespace engine {

	enum FrequencyAccess {
		Stream,
		Static,
		Dynamic
	};

	enum NatureAccess {
		Draw,
		Read,
		Copy
	};

	enum BufferType {
		VertexBuffer,
		IndexBuffer
	};

	class HardwareBuffer : public Buffer {
	public:
		HardwareBuffer(int count, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess);
		virtual ~HardwareBuffer();

		virtual void bind();
		virtual void unbind();

		virtual int getCount() const;
		virtual void* getPointer() const;
		virtual bool isOk() const;

		virtual void* map(AccessType accessType);
		virtual void unmap();

	private:
		void ensureBuffer();
		void destroyBuffer();
		int getTarget(BufferType type);
		int getUsage(FrequencyAccess frequencyAccess, NatureAccess natureAccess);
		int getAccess(AccessType accessType);

		unsigned int id;
		int count;
		int target;
		int usage;
	};

} /* namespace engine */

#endif /* HARDWAREBUFFER_H_ */
