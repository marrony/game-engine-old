/*
 * ResourceLoader.h
 *
 *  Created on: 01/08/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCELOADER_H_
#define RESOURCELOADER_H_

namespace engine {

	class Resource;

	struct ResourceLoader {
		virtual ~ResourceLoader() {}

		virtual Resource* loadResource(const std::string& name) = 0;
	};

}  // namespace engine


#endif /* RESOURCELOADER_H_ */
