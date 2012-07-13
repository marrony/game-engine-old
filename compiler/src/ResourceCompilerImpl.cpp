/*
 * ResourceCompilerImpl.cpp
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */

#include "ResourceCompilerImpl.h"
#include "Exception.h"
#include "Resource.h"
#include "Stream.h"
#include "FileUtil.h"

namespace compiler {

	ResourceCompilerImpl::ResourceCompilerImpl() {
	}

	ResourceCompilerImpl::~ResourceCompilerImpl() {
	}

	void ResourceCompilerImpl::registerLoader(ResourceLoader* loader, const char* name, const char* ext) {
		size_t begin = 0;

		std::string extensions(ext);

		while(begin < extensions.size()) {
			size_t end = extensions.find(';', begin);

			if(end == std::string::npos)
				end = extensions.size();

			std::string extension = extensions.substr(begin, end - begin);

			loaders.insert({engine::Hashed(ext), loader});

			begin = end + 1;
		}
	}

	ResourceLoader* ResourceCompilerImpl::findLoader(const char* extension) {
		auto ite = loaders.find(engine::Hashed(extension));

		if(ite != loaders.end())
			return ite->second;

		throw engine::Exception("Loader not found");
	}

	bool ResourceCompilerImpl::compile(const char* fileName, std::map<std::string, std::string>& options) {
		std::cout << "Compiling " << fileName << std::endl;

		engine::Hashed id(fileName);

		if(resourcesCompiled.count(id) > 0) {
			std::cout << fileName << " already compiled" << std::endl;
			return true;
		}

		ResourceLoader* loader = findLoader(engine::file::getExtension(fileName).c_str());

		resourcesCompiled.insert(id);
		loader->compileResource(fileName, options);

		std::cout << "Compiled " << fileName << std::endl;
		return true;
	}

} /* namespace compiler */
