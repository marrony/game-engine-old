/*
 * DynamicLibrary.cpp
 *
 *  Created on: 29/04/2012
 *      Author: Marrony
 */

#include "DynamicLibrary.h"
#include "System.h"

#ifndef WIN32
#include <dlfcn.h>
#endif

namespace engine {

	DynamicLibrary::DynamicLibrary(const std::string& name) :
			name(name), hInstance(0) {
	}

	DynamicLibrary::~DynamicLibrary() {
#ifdef WIN32
//TODO Windows 64 nao ta funcionando
//			if(hInstance)
//				FreeLibrary((HINSTANCE) hInstance);
#else
		dlclose(hInstance);
#endif
	}

	bool DynamicLibrary::getName(char* name, int maxLenght) const {
#ifdef WIN32
		return GetModuleFileName((HINSTANCE) hInstance, name, maxLenght) != 0;
#else
		return true;
#endif
	}

	void* DynamicLibrary::getProcAddress(const char* procName) {
#ifdef WIN32
		if(!hInstance)
			hInstance = (HINSTANCE) LoadLibrary((name + ".dll").c_str());

		return (void*) GetProcAddress((HINSTANCE) hInstance, procName);
#else
		if(!hInstance)
			hInstance = dlopen((name + ".so").c_str(), RTLD_LAZY);

		return (void*) dlsym(hInstance, procName);
#endif
	}

} /* namespace engine */
