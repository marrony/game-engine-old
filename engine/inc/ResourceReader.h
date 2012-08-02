/*
 * ResourceReader.h
 *
 *  Created on: 01/08/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCEREADER_H_
#define RESOURCEREADER_H_

namespace engine {

	class Resource;

	struct ResourceReader {
		virtual ~ResourceReader() {}

		virtual Resource* readResource(const std::string& name) = 0;
	};

}  // namespace engine


#endif /* RESOURCEREADER_H_ */
