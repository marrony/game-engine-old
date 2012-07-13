/*
 * ResourceCompilerImpl.h
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCECOMPILERIMPL_H_
#define RESOURCECOMPILERIMPL_H_

#include "ResourceCompiler.h"
#include "ResourcePlugin.h"
#include "ResourceLoader.h"
#include "ResourceManager.h"
#include "Hashed.h"

#include <string>
#include <tuple>
#include <map>
#include <set>

namespace compiler {

	class ResourceCompilerImpl : public ResourceCompiler {
	public:
		ResourceCompilerImpl();
		virtual ~ResourceCompilerImpl();

		virtual void registerLoader(ResourceLoader* loader, const char* name, const char* extensions);
		virtual ResourceLoader* findLoader(const char* extension);

		virtual bool compile(const char* fileName, std::map<std::string, std::string>& options);

		virtual engine::ResourceId findResource(const char* name, engine::Type type);
		virtual engine::ResourceId addResource(ResourceLoader* loader, engine::Resource* resource);
		virtual engine::Resource* getResource(engine::ResourceId resourceId);

		void addWriter(engine::Type type, engine::ResourceWriter writer, const char* prefix);

		void saveResources();
	private:
		void deleteUnusedResources();

		engine::ResourceManager resourceManager;
		std::map<engine::Hashed, ResourceLoader*> loaders;
		std::map<engine::Type, engine::ResourceWriter> writers;
		std::map<engine::Type, const char*> prefixes;
		std::map<engine::Hashed, std::tuple<engine::Resource*, ResourceLoader*>> resources;
		std::set<engine::Hashed> resourcesCompiled;
	};

} /* namespace compiler */
#endif /* RESOURCECOMPILER_H_ */
