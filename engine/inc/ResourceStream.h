/*
 * ResourceStream.h
 *
 *  Created on: 11/05/2012
 *      Author: objective
 */

#ifndef RESOURCESTREAM_H_
#define RESOURCESTREAM_H_

#include <string>

namespace engine {

	struct ResourceStream {
		virtual ~ResourceStream() {
		}

		virtual void pushGroup(const char* label) = 0;
		virtual void popGroup() = 0;

		virtual void writeString(const char* label, const std::string& value) = 0;
		virtual void writeInt(const char* label, int value) = 0;
		virtual void writeShort(const char* label, short value) = 0;
		virtual void writeByte(const char* label, char value) = 0;
		virtual void writeFloat(const char* label, float value) = 0;
		virtual void writeArray(const char* label, const int* array, size_t count) = 0;
		virtual void writeArray(const char* label, const short* array, size_t count) = 0;
		virtual void writeArray(const char* label, const char* array, size_t count) = 0;
		virtual void writeArray(const char* label, const float* array, size_t count) = 0;

		virtual std::string readString(const char* label) = 0;
		virtual int readInt(const char* label) = 0;
		virtual short readShort(const char* label) = 0;
		virtual char readByte(const char* label) = 0;
		virtual float readFloat(const char* label) = 0;
		virtual void readArray(const char* label, int* array, size_t count) = 0;
		virtual void readArray(const char* label, short* array, size_t count) = 0;
		virtual void readArray(const char* label, char* array, size_t count) = 0;
		virtual void readArray(const char* label, float* array, size_t count) = 0;
	};

}  // namespace engine


#endif /* RESOURCESTREAM_H_ */
