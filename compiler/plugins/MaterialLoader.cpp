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
#include "Effect.h"
#include "Image.h"
#include "FileUtil.h"
#include "tinyxml.h"
#include "Stream.h"

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

class MaterialLoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;
public:
	MaterialLoader() :
			compiler(0) {
	}

	virtual ~MaterialLoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		this->manager = manager;

		compiler->registerLoader(this, "Material Loader", "material.xml");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		TiXmlDocument document;
		document.LoadFile(fileName);

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
		Effect* effect = (Effect*)manager->loadResource(EffectKey(effectName));

		Material* material = new Material(file::getFilename(fileName), manager, effect);

		TiXmlElement* xmlSampler = xmlEffect->FirstChildElement("sampler");
		while(xmlSampler) {
			const char* samplerName = xmlSampler->Attribute("name");
			const char* textureSrc = xmlSampler->Attribute("src");
			std::string textureName = file::getFilename(textureSrc);

			compiler->compile(textureSrc, options);

			Image* image = (Image*)manager->loadResource(ImageKey(textureName));
			material->addSampler(samplerName, image);

			xmlSampler = xmlSampler->NextSiblingElement("sampler");
		}

		std::string outputName = file::getPath(fileName) + "/" + file::getFilename(fileName) + ".material";
		FileStream fileStream(outputName);
		ResourceBinStream resourceStream(fileStream);
		MaterialUtils::write(resourceStream, *manager, material);

		delete material;
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
