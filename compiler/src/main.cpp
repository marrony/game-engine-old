/*
 * main.cpp
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */
#include "ResourcePlugin.h"
#include "ResourceCompilerImpl.h"
#include "PluginManager.h"
#include "ResourceManager.h"

#include "Image.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "Scene.h"
#include "tinyxml.h"
#include "FileUtil.h"
#include "Exception.h"

#include <map>
#include <vector>
#include <string>
#include <fstream>

using namespace engine;
using namespace compiler;

#include <sys/unistd.h>
#include <stdlib.h>

#include "MemoryManager.h"

extern "C" Module* getModule() {
	return 0;
}

void parseOptions(TiXmlElement* xmlOptions, std::map<std::string, std::string>& options) {
	if(!xmlOptions) return;

	TiXmlElement* xmlOption = xmlOptions->FirstChildElement("option");
	while(xmlOption) {
		const char* name = xmlOption->Attribute("name");
		const char* value = xmlOption->Attribute("value");

		if(name)
			options[name] = value ? value : "";

		xmlOption = xmlOption->NextSiblingElement("option");
	}
}

int compile(PluginManager& pluginManager) {
	ResourceCompilerImpl compiler;

	compiler.addWriter(Material::TYPE, MaterialUtils::write, "resources/materials");
	compiler.addWriter(Model::TYPE, ModelUtils::write, "resources/scene");
	compiler.addWriter(Effect::TYPE, EffectUtils::write, "resources/materials");
	compiler.addWriter(Texture::TYPE, TextureUtils::write, "resources/images");
	compiler.addWriter(Scene::TYPE, SceneUtils::write, "resources/scene");

	std::vector<Plugin*> plugins = pluginManager.getPlugins();
	for(size_t i = 0; i < plugins.size(); i++) {
		ResourceLoader* loader = dynamic_cast<ResourceLoader*>(plugins[i]);

		if(loader)
			loader->initialize(&compiler);
	}

	TiXmlDocument document;
	document.LoadFile("resources/resources.xml");

	try {
		TiXmlElement* xmlResources = document.RootElement();

		TiXmlElement* xmlResource = xmlResources->FirstChildElement("resource");
		while(xmlResource) {
			const char* src = xmlResource->Attribute("src");
			std::map<std::string, std::string> options;

			parseOptions(xmlResource->FirstChildElement("options"), options);

			compiler.compile(src, options);

			xmlResource = xmlResource->NextSiblingElement("resource");
		}

		compiler.saveResources();
	} catch(const Exception& e) {
		std::cout << e.getMessage() << std::endl;
	}

	std::cout << "Finished" << std::endl;

	return 0;
}

int main(int argc, char* argv[]) {
	PluginManager pluginManager;

	pluginManager.loadPlugin("libcollada-loader");
	pluginManager.loadPlugin("libdotmesh-loader");
	pluginManager.loadPlugin("libms3d-loader");
	pluginManager.loadPlugin("libeffect-loader");
	pluginManager.loadPlugin("libmaterial-loader");
	pluginManager.loadPlugin("libtga-loader");
	pluginManager.loadPlugin("libdotscene-loader");

	return compile(pluginManager);
}
