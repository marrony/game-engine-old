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

	class ResourceManager {
		std::vector<class ResourceListener*> listeners;

		struct ResourceEntry {
			class Resource* resource;
			int count;
		};

		std::map<std::string, ResourceEntry> resources;
	public:
		ResourceManager();
		~ResourceManager();

		class Resource* loadResource(const class ResourceKey& key);
		void unloadResource(class Resource* resource);

		void dispatchLoadedEvent(const class ResourceEvent& event);
		void dispatchUnloadedEvent(const class ResourceEvent& event);

		void addListener(class ResourceListener* listener);
		void removeListener(class ResourceListener* listener);
	};

} /* namespace engine */

#endif /* RESOURCEMANAGER_H_ */
