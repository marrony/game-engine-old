/*
 * Stream.h
 *
 *  Created on: 27/03/2012
 *      Author: marrony.neris
 */

#ifndef STREAM_H_
#define STREAM_H_

#include <vector>
#include <string>
#include <stdio.h>

namespace engine {

	class Stream {
	public:
		enum SeekType {
			BEGIN,
			END,
			CURRENT
		};

		virtual ~Stream() { }

		virtual int readByte() {
			char byte;

			read(&byte, 1);

			return byte;
		}

		virtual void read(void* buffer, size_t size) = 0;

		void writeByte(char byte) {
			write(&byte, 1);
		}

		virtual void write(const void* buffer, size_t size) = 0;

		virtual void seek(int offset, SeekType seekType) = 0;

		virtual int getPosition() = 0;
	};

	class MemoryStream : public Stream {
	public:
		MemoryStream(int capacity = 0);

		MemoryStream(const char* data, int size);

		virtual void read(void* buffer, size_t size);

		virtual void write(const void* buffer, size_t size);

		virtual void seek(int offset, SeekType seekType);

		virtual int getPosition();
	private:
		std::vector<char> buf;
		int position;
	};

	class FileStream : public Stream {
	public:
		FileStream(const std::string& fileName);

		virtual ~FileStream();

		virtual void read(void* buffer, size_t size);

		virtual void write(const void* buffer, size_t size);

		virtual void seek(int offset, SeekType seekType);

		virtual int getPosition();
	private:
		FILE* file;
	};

} /* namespace engine */
#endif /* STREAM_H_ */
