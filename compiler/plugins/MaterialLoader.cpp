/*
 * MaterialLoader.cpp
 *
 *  Created on: 30/03/2012
 *      Author: marrony.neris
 */

#include "ResourcePlugin.h"
#include "ResourceCompiler.h"
#include "Exception.h"
#include "Material.h"
#include "Texture.h"
#include "FileUtil.h"
#include "tinyxml.h"

#include <fstream>

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

class MaterialLoader : public ResourceLoader {
	std::vector<Resource*> resources;
	ResourceCompiler* compiler;
public:
	MaterialLoader() :
			resources(), compiler(0) {
	}

	virtual ~MaterialLoader() {
		for(size_t i = 0; i < resources.size(); i++)
			delete resources[i];
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler) {
		this->compiler = compiler;

		compiler->registerLoader(this, "Material Loader", "material.xml");
	}

	virtual void loadResource(const char* fileName, std::map<std::string, std::string>& options) {
		std::ifstream stream(fileName);
		std::string content = file::loadFromStream(stream);

		TiXmlDocument document;

		document.Parse(content.c_str());

		if(document.Error()) {
			throw Exception(document.ErrorDesc());
		}

		TiXmlElement* rootMaterial = document.RootElement();
		if(strcmp("material", rootMaterial->Value()) != 0) {
			throw Exception("");
		}

		TiXmlElement* xmlEffect = rootMaterial->FirstChildElement("effect");
		if(!xmlEffect)
			throw Exception("");

		const char* effectSrc = xmlEffect->Attribute("src");
		std::string effectName = file::getFilename(effectSrc);

		compiler->compile(effectSrc, options);
		engine::ResourceId effectId = compiler->findResource(effectName.c_str(), Effect::TYPE);

		if(effectId == engine::ResourceId(-1))
			throw Exception("Effect not found");

		Material* material = new Material(file::getFilename(fileName), effectId);

		TiXmlElement* xmlSampler = xmlEffect->FirstChildElement("sampler");
		while(xmlSampler) {
			const char* samplerName = xmlSampler->Attribute("name");
			const char* textureSrc = xmlSampler->Attribute("src");
			std::string textureName = file::getFilename(textureSrc);

			compiler->compile(textureSrc, options);
			engine::ResourceId textureId = compiler->findResource(textureName.c_str(), Texture::TYPE);

			if(textureId == engine::ResourceId(-1)) {
				delete material;

				throw Exception("Texture not found");
			}

			material->addSampler(samplerName, textureId);

			xmlSampler = xmlSampler->NextSiblingElement("sampler");
		}

		resources.push_back(material);

		compiler->addResource(this, material);
	}

	virtual void destroyResource(engine::Resource* resource) {
		std::vector<Resource*>::iterator ite = std::find(resources.begin(), resources.end(), resource);

		if(ite != resources.end()) {
			resources.erase(ite);
			delete resource;
		}
	}
};

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new MaterialLoader;
}
