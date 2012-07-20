/*
 * ResourceManager.h
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include "Resource.h"

#include <map>
#include <vector>
#include <string>
#include <functional>

namespace engine {

	class ResourceManager {
		std::vector<ResourceListener*> listeners;

		struct ResourceEntry {
			Resource* resource;
			int count;
		};

		std::map<std::string, ResourceEntry> resources;
	public:
		ResourceManager();
		~ResourceManager();

		Resource* loadResource(const ResourceKey& key);
		void unloadResource(Resource* resource);

		void dispatchLoadedEvent(const ResourceEvent& event);
		void dispatchUnloadedEvent(const ResourceEvent& event);

		void addListener(ResourceListener* listener);
		void removeListener(ResourceListener* listener);
	};

} /* namespace engine */

#endif /* RESOURCEMANAGER_H_ */
