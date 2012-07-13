/*
 * Exception.h
 *
 *  Created on: 09/02/2011
 *      Author: marrony
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>
#include <sstream>

namespace engine {

	class Exception {
		std::string message;
	public:
		Exception(const std::string& m, const char* file, int line) {
			std::stringstream stream;

			stream << file << "(" << line << "): " << m;

			message = stream.str();
		}

		std::string getMessage() const {
			return message;
		}
	};

}  // namespace engine

#define Exception(message) Exception(message, __FILE__, __LINE__)

#endif /* EXCEPTION_H_ */
