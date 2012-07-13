/*
 * PluginManager.cpp
 *
 *  Created on: 29/03/2012
 *      Author: marrony.neris
 */

#include "PluginManager.h"
#include "DynamicLibrary.h"
#include "MemoryManager.h"

namespace engine {

	PluginManager::PluginManager() {
	}

	PluginManager::~PluginManager() {
		for(size_t i = 0; i < plugins.size(); i++) {
			Module* dynLib = plugins[i].first;
			Plugin* plugin = plugins[i].second;

			plugin->release();

			delete dynLib;
		}
	}

	void PluginManager::loadPlugin(const std::string& pluginName) {
		typedef Plugin* (*InitFunc)(Module*);

		Module* dynLib = new DynamicLibrary(pluginName);

		InitFunc initFunc = (InitFunc) dynLib->getProcAddress("initializePlugin");

		Plugin* plugin = initFunc(dynLib);

		plugins.push_back(std::make_pair(dynLib, plugin));
	}

	std::vector<Plugin*> PluginManager::getPlugins() const {
		std::vector<Plugin*> p;

		for(size_t i = 0; i < plugins.size(); i++)
			p.push_back(plugins[i].second);

		return p;
	}

} /* namespace engine */
