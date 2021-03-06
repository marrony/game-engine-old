/*
 * ResourceManager.h
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <map>
#include <vector>
#include <string>

namespace engine {

	class Resource;
	class ResourceEvent;
	class ResourceKey;
	class ResourceListener;

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
