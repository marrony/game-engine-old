/*
 * ResourceManager.cpp
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#include "ResourceManager.h"
#include "Resource.h"

#include <algorithm>

namespace engine {

	ResourceManager::ResourceManager() {
	}

	ResourceManager::~ResourceManager() {
	}

	Resource* ResourceManager::loadResource(const ResourceKey& key) {
		std::string resourceName = key.getKeyName();

		auto entry = resources.find(resourceName);

		Resource* resource;

		if(entry != resources.end())
			resource = entry->second.resource;
		else {
			resource = key.loadResource(*this);
			entry = resources.insert({resourceName, {resource, 0}}).first;
		}

		entry->second.count++;

		return resource;
	}

	void ResourceManager::unloadResource(Resource* resource) {
		std::string resourceName = resource->getName();

		auto entry = resources.find(resourceName);

		if(entry == resources.end()) return;

		if(--entry->second.count > 0) return;

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
