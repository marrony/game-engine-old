/*
 * ResourceLoader.h
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCELOADER_H_
#define RESOURCELOADER_H_

#include "ResourcePlugin.h"
#include "Resource.h"

#include <string>
#include <map>

namespace compiler {

	struct ResourceCompiler;

	struct ResourceLoader : public Plugin {
		virtual ~ResourceLoader() { }

		virtual void initialize(ResourceCompiler* compiler, class engine::ResourceManager* manager) = 0;

		virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) = 0;
		virtual void destroyResource(engine::Resource* resource) = 0;
	};

}  // namespace compiler

#endif /* RESOURCELOADER_H_ */
