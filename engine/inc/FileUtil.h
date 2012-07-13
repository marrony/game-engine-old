/*
 * FileUtil.h
 *
 *  Created on: 09/02/2011
 *      Author: marrony
 */

#ifndef FILEUTIL_H_
#define FILEUTIL_H_

#include <iostream>
#include <sstream>
#include <iterator>

#include <sys/stat.h>

namespace engine {

	namespace file {

		static inline std::string getExtension(const std::string& fullPath) {
			size_t position = fullPath.find_first_of('.');

			return fullPath.substr(position + 1, fullPath.size() - position + 1);
		}

		static inline std::string getFilename(const std::string& fullPath) {
			size_t slashPosition = fullPath.find_last_of("/\\");
			size_t dotPosition = fullPath.find_first_of('.');

			return fullPath.substr(slashPosition + 1, dotPosition - (slashPosition + 1));
		}

		static inline std::string getPath(const std::string& fullPath) {
			size_t slashPosition = fullPath.find_last_of("/\\");

			return fullPath.substr(0, slashPosition);
		}

		static inline std::string loadFromStream(std::istream& stream) {
			std::stringstream buffer;

			stream.unsetf(stream.skipws);
			std::istream_iterator<char> beginOfStream(stream);
			std::istream_iterator<char> endOfStream;
			std::ostream_iterator<char> bufferStream(buffer);

			std::copy(beginOfStream, endOfStream, bufferStream);

			return buffer.str();
		}

		static inline bool fileExists(const std::string& fileName) {
#ifdef WIN32
			struct _stat st;
			return _stat(fileName.c_str(), &st) == 0;
#else
			struct stat st;
			return stat(fileName.c_str(), &st) == 0;
#endif
		}

		static inline bool isMoreRecent(const std::string& file0, const std::string& file1) {
#ifdef WIN32
			struct _stat st0;
			struct _stat st1;

			if(_stat(file0.c_str(), &st0) != 0)
				return false;

			if(_stat(file1.c_str(), &st1) != 0)
				return true;
#else
			struct stat st0;
			struct stat st1;

			if(stat(file0.c_str(), &st0) != 0)
				return false;

			if(stat(file1.c_str(), &st1) != 0)
				return true;
#endif

			return st0.st_mtime > st1.st_mtime;
		}

	}  // namespace file

}  // namespace engine

#endif /* FILEUTIL_H_ */
