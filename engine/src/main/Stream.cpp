/*
 * Stream.cpp
 *
 *  Created on: 27/03/2012
 *      Author: marrony.neris
 */

#include "Stream.h"

#include "Exception.h"
#include "string.h"

namespace engine {

	MemoryStream::MemoryStream(int capacity) : buf(capacity), position(0) { }

	MemoryStream::MemoryStream(const char* data, int size) : buf(data, data+size), position(0) { }

	void MemoryStream::read(void* buffer, size_t size) {
		if(position + size > buf.size())
			throw Exception("");

		memcpy(buffer, &buf[position], size);
		position += size;
	}

	void MemoryStream::write(const void* buffer, size_t size) {
		size_t newSize = position + size;

		if(newSize > buf.size())
			buf.resize(std::max(buf.size() << 1, newSize), 0);

		memcpy(&buf[position], buffer, size);
		position += size;
	}

	void MemoryStream::seek(int offset, SeekType seekType) {
		switch(seekType) {
		case Stream::BEGIN:
			position = offset;
			break;

		case Stream::END:
			position = buf.size() - position;
			break;

		case Stream::CURRENT:
			position += offset;
			break;
		}
	}

	int MemoryStream::getPosition() {
		return position;
	}


	FileStream::FileStream(const std::string& fileName) : file(0) {
		file = fopen(fileName.c_str(), "r+b");

		if(!file)
			file = fopen(fileName.c_str(), "w+b");
	}

	FileStream::~FileStream() {
		fclose(file);
	}

	void FileStream::read(void* buffer, size_t size) {
		fread(buffer, sizeof(char), size, file);
	}

	void FileStream::write(const void* buffer, size_t size) {
		fwrite(buffer, sizeof(char), size, file);
	}

	void FileStream::seek(int offset, SeekType seekType) {
		switch(seekType) {
		case Stream::BEGIN:
			fseek(file, offset, SEEK_SET);
			break;

		case Stream::END:
			fseek(file, offset, SEEK_END);
			break;

		case Stream::CURRENT:
			fseek(file, offset, SEEK_CUR);
			break;
		}
	}

	int FileStream::getPosition() {
		return ftell(file);
	}

} /* namespace engine */
