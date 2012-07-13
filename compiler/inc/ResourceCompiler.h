/*
 * ResourceCompiler.h
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCECOMPILER_H_
#define RESOURCECOMPILER_H_

#include "ResourcePlugin.h"
#include "ResourceLoader.h"
#include "ResourceManager.h"

#include <string>
#include <map>

namespace compiler {

	struct ResourceCompiler {
		virtual ~ResourceCompiler() { };

		virtual void registerLoader(ResourceLoader* loader, const char* name, const char* extensions) = 0;
		virtual ResourceLoader* findLoader(const char* extension) = 0;

		virtual bool compile(const char* fileName, std::map<std::string, std::string>& options) = 0;
	};

} /* namespace compiler */
#endif /* RESOURCECOMPILER_H_ */
