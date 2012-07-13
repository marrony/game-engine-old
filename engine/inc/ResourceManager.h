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

	typedef unsigned int ResourceId;

	class ResourceManager;

	template<typename R>
	class ResourceGuard {
//	public:
//		ResourceGuard(ResourceId id) :
//				id(id) {
//			lock();
//		}
//
//		ResourceGuard(const ResourceGuard& other) :
//				id(other.id), manager(other.manager) {
//			lock();
//		}
//
//		~ResourceGuard() {
//			unlock();
//		}
//
//		R* operator->() const {
//			return get();
//		}
//
//		R& operator*() const {
//			return *get();
//		}
//
//		R* get() const {
//			return (R*)manager.getResource(id);
//		}
//	private:
//		void lock() {
//			if(resource)
//				manager.incrementReference(id);
//		}
//
//		void unlock() {
//			if(resource)
//				manager.decrementReference(id);
//		}
//
//		ResourceId id;
//		ResourceManager& manager;
	};

	class ResourceManager {
	public:
		ResourceManager();
		~ResourceManager();

		void registerFactory(Type type, ResourceFactory factory);

		void registerReader(Type type, const char* prefix, ResourceReader reader);
		void registerWriter(Type type, const char* prefix, ResourceWriter writer);

		ResourceReader getReader(Type type);
		ResourceWriter getWriter(Type type);

		ResourceId addResource(Resource* resource);

		ResourceId registerResource(const std::string& resourceName, Type resourceType);

		Resource* getResource(ResourceId id);

//		void incrementReference(ResourceId id);
//
//		void decrementReference(ResourceId id);

		ResourceId findResource(Type type, const std::string& name);

		void clearResources();

		void deleteUnusedResources();
	private:
		struct ResourceEntry {
			bool used;
			Type type;
			std::string name;
			Resource* resource;

			ResourceEntry() :
					used(false), type(0), name(), resource(0) {
			}
		};

		std::vector<ResourceEntry> resources;
		std::map<Type, const char*> readerPrefix;
		std::map<Type, ResourceReader> readers;
		std::map<Type, const char*> writerPrefix;
		std::map<Type, ResourceWriter> writers;

		std::map<Type, ResourceFactory> factories;
	};

} /* namespace engine */
#endif /* RESOURCEMANAGER_H_ */
