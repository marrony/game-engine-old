/*
 * DynamicLibrary.h
 *
 *  Created on: 29/04/2012
 *      Author: Marrony
 */

#ifndef DYNAMICLIBRARY_H_
#define DYNAMICLIBRARY_H_

#include "Module.h"

#include <string>

namespace engine {

	struct DynamicLibrary : public Module {
		DynamicLibrary(const std::string& name);

		virtual ~DynamicLibrary();

		virtual bool getName(char* name, int maxLenght) const;

		virtual void* getProcAddress(const char* procName);

		std::string name;
		void* hInstance;
	};

} /* namespace engine */

#endif /* DYNAMICLIBRARY_H_ */
