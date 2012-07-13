/*
 * PluginManager.h
 *
 *  Created on: 29/03/2012
 *      Author: marrony.neris
 */

#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_

#include "ResourcePlugin.h"

#include "System.h"

#include <vector>
#include <string>

namespace engine {

	class PluginManager {
	public:
		PluginManager();
		~PluginManager();

		void loadPlugin(const std::string& plugin);

		std::vector<Plugin*> getPlugins() const;
	private:
		std::vector<std::pair<Module*, Plugin*> > plugins;
	};

} /* namespace engine */

#endif /* PLUGINMANAGER_H_ */
