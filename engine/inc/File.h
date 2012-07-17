/*
 * File.h
 *
 *  Created on: 16/07/2012
 *      Author: marrony
 */

#ifndef FILE_H_
#define FILE_H_

#include <string>

namespace engine {

	class File {
	public:
		std::string getPath();
		std::string getName();
	};

} /* namespace engine */

#endif /* FILE_H_ */
