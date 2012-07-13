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
		resourceManager.clearResources();

		deleteUnusedResources();
	}

	void ResourceCompilerImpl::deleteUnusedResources() {
		std::vector<std::map<engine::Hashed, std::tuple<engine::Resource*, ResourceLoader*>>::iterator> resourcesToDelete;

		for(auto entry = resources.begin(); entry != resources.end(); entry++) {
			if(std::get<0>(entry->second)->couldDelete())
				resourcesToDelete.push_back(entry);
		}

		for(size_t i = 0; i < resourcesToDelete.size(); i++) {
			ResourceLoader* loader = std::get<1>(resourcesToDelete[i]->second);
			engine::Resource* resource = std::get<0>(resourcesToDelete[i]->second);

			loader->destroyResource(resource);

			resources.erase(resourcesToDelete[i]);
		}

		if(!resourcesToDelete.empty())
			deleteUnusedResources();
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

		resourcesCompiled.find(id);

		if(resourcesCompiled.count(id) > 0) {
			std::cout << fileName << " already compiled" << std::endl;
			return true;
		}

		ResourceLoader* loader = findLoader(engine::file::getExtension(fileName).c_str());

		resourcesCompiled.insert(id);
		loader->loadResource(fileName, options);

		std::cout << "Compiled " << fileName << std::endl;
		return true;
	}

	engine::ResourceId ResourceCompilerImpl::findResource(const char* name, engine::Type type) {
		return resourceManager.findResource(type, name);
	}

	engine::ResourceId ResourceCompilerImpl::addResource(ResourceLoader* loader, engine::Resource* resource) {
		engine::ResourceId resourceId = resourceManager.addResource(resource);

		engine::Hashed id((resource->getName() + "/" + resource->getType().getName()).c_str());
		resources[id] = std::make_tuple(resource, loader);

		return resourceId;
	}

	engine::Resource* ResourceCompilerImpl::getResource(engine::ResourceId resourceId) {
		return resourceManager.getResource(resourceId);
	}

	void ResourceCompilerImpl::addWriter(engine::Type type, engine::ResourceWriter writer, const char* prefix) {
		writers[type] = writer;
		prefixes[type] = prefix;
	}

	void ResourceCompilerImpl::saveResources() {
		for(auto ite : resources) {
			engine::Resource* resource = std::get<0>(ite.second);

			std::string prefix = prefixes[resource->getType()];
			std::string fileName = prefix + "/" + resource->getName() + "." + resource->getType().getName();

			engine::FileStream stream(fileName);
			engine::ResourceBinStream resourceStream(stream);

			std::map<engine::Type, engine::ResourceWriter>::iterator writer = writers.find(resource->getType());
			if(writer != writers.end())
				writer->second(resourceStream, resourceManager, resource);
		}
	}

} /* namespace compiler */
