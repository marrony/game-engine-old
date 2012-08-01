/*
 * ResourceManager.cpp
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#include "ResourceManager.h"
#include "ResourceLoader.h"
#include "Resource.h"

#include <algorithm>

namespace engine {

	ResourceManager::ResourceManager() {
	}

	ResourceManager::~ResourceManager() {
	}

	void ResourceManager::registerLoader(const std::string& type, ResourceLoader* loader) {
		loaders[type] = loader;
	}

	Resource* ResourceManager::loadResource(const std::string& type, const std::string& name) {
		std::string resourceName = type + "/" + name;

		auto entry = resources.find(resourceName);

		Resource* resource;

		if(entry != resources.end())
			resource = entry->second.resource;
		else {
			ResourceLoader* loader = loaders[type];

			if(loader == nullptr) return 0;

			resource = loader->loadResource(name);

			ResourceEvent event;
			event.type = type;
			event.resource = resource;
			dispatchLoadedEvent(event);

			entry = resources.insert({resourceName, {resource, 0}}).first;
		}

		entry->second.count++;

		return resource;
	}

	Resource* ResourceManager::loadResource(const ResourceKey& key) {
		std::string resourceName = key.getKeyName();

		auto entry = resources.find(resourceName);

		Resource* resource;

		if(entry != resources.end())
			resource = entry->second.resource;
		else {
			resource = key.loadResource(*this);

			ResourceEvent event;
			event.type = resource->getType().getName();
			event.resource = resource;
			dispatchLoadedEvent(event);

			entry = resources.insert({resourceName, {resource, 0}}).first;
		}

		entry->second.count++;

		return resource;
	}

	void ResourceManager::unloadResource(Resource* resource) {
		std::string resourceName = resource->getKeyName();

		auto entry = resources.find(resourceName);

		if(entry == resources.end()) return;

		if(--entry->second.count > 0) return;

		ResourceEvent event;
		event.type = resource->getType().getName();
		event.resource = resource;
		dispatchUnloadedEvent(event);

		delete entry->second.resource;

		resources.erase(entry);
	}

	void ResourceManager::dispatchLoadedEvent(const ResourceEvent& event) {
		for(ResourceListener* listener : listeners)
			listener->onResourceLoaded(event);
	}

	void ResourceManager::dispatchUnloadedEvent(const ResourceEvent& event) {
		for(ResourceListener* listener : listeners)
			listener->onResourceUnloaded(event);
	}

	void ResourceManager::addListener(ResourceListener* listener) {
		listeners.push_back(listener);
	}

	void ResourceManager::removeListener(ResourceListener* listener) {
		auto ite = std::find(listeners.begin(), listeners.end(), listener);

		if(ite != listeners.end())
			listeners.erase(ite);
	}

} /* namespace engine */
