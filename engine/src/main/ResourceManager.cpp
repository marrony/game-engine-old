/*
 * ResourceManager.cpp
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#include "ResourceManager.h"
#include "GeneralHashFunction.h"
#include "Exception.h"
#include "FileUtil.h"

#include <fstream>
#include <string.h>

namespace engine {

	ResourceManager::ResourceManager() {
		resources.reserve(1024); //FIXME o resize do vector esta dando erro, por isso ja esta reservando esse valor
	}

	ResourceManager::~ResourceManager() {
		deleteUnusedResources();

//		if(!resources.empty())
//			throw Exception("There are some resources that could not be released");
	}

	void ResourceManager::registerFactory(Type type, ResourceFactory factory) {
		factories[type] = factory;
	}

	void ResourceManager::registerReader(Type type, const char* prefix, ResourceReader reader) {
		readerPrefix[type] = prefix;
		readers[type] = reader;
	}

	void ResourceManager::registerWriter(Type type, const char* prefix, ResourceWriter writer) {
		writerPrefix[type] = prefix;
		writers[type] = writer;
	}

	ResourceReader ResourceManager::getReader(Type type) {
		return readers[type];
	}

	ResourceWriter ResourceManager::getWriter(Type type) {
		return writers[type];
	}

	ResourceId ResourceManager::addResource(Resource* resource) {
		ResourceId id = registerResource(resource->getName(), resource->getType());

		resources[id].resource = resource;

		return id;
	}

	ResourceId ResourceManager::registerResource(const std::string& resourceName, Type resourceType) {
		ResourceId id = findResource(resourceType, resourceName);

		if(id == -1) {
			for(id = 0; id < resources.size(); ++id) {
				if(!resources[id].used)
					break;
			}

			if(id >= resources.size()) {
				id = resources.size();
				resources.push_back(ResourceEntry());
			}

			ResourceEntry& entry = resources[id];

			entry.used = true;
			entry.name = resourceName;
			entry.type = resourceType;
		}

		return id;
	}

	Resource* ResourceManager::getResource(ResourceId id) {
		if(id >= resources.size())
			return 0;

		ResourceEntry& entry = resources[id];

		if(!entry.resource) {
			ResourceReader reader = getReader(entry.type);

			const char* prefix = readerPrefix[entry.type];

			FileStream fileStream(std::string(prefix) + "/" + entry.name + "." + entry.type.getName());

			ResourceBinStream resourceStream(fileStream);
			entry.resource = (Resource*)reader(resourceStream, *this, 0);

			entry.resource->postLoaded();
		}

		return entry.resource;
	}

	ResourceId ResourceManager::findResource(Type type, const std::string& name) {
		for(ResourceId id = 0; id < resources.size(); id++) {
			if(resources[id].name == name && resources[id].type == type)
				return id;
		}

		return ResourceId(-1);
	}

	void ResourceManager::clearResources() {
		for(ResourceEntry& entry : resources) {
			entry.used = false;
			entry.resource = 0;
		}
		resources.clear();
	}

	void ResourceManager::deleteUnusedResources() {
		std::vector<ResourceId> resourcesToDelete;

		for(ResourceId id = 0; id < resources.size(); id++) {
			bool shouldDelete = resources[id].used &&
							resources[id].resource &&
							resources[id].resource->couldDelete();

			if(shouldDelete)
				resourcesToDelete.push_back(id);
		}

		for(ResourceId id : resourcesToDelete) {
			delete resources[id].resource;

			resources[id].used = false;
			resources[id].resource = 0;
		}

		if(!resourcesToDelete.empty())
			deleteUnusedResources();
	}

} /* namespace engine */
