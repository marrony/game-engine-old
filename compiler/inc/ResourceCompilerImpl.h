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
	private:
		std::map<engine::Hashed, ResourceLoader*> loaders;
		std::set<engine::Hashed> resourcesCompiled;
	};

} /* namespace compiler */

#endif /* RESOURCECOMPILER_H_ */
