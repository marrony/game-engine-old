/*
 * EffectLoader.cpp
 *
 *  Created on: 30/03/2012
 *      Author: marrony.neris
 */
#include "ResourcePlugin.h"
#include "ResourceCompiler.h"
#include "Exception.h"
#include "FileUtil.h"
#include "Effect.h"
#include "Shader.h"

#include "tinyxml.h"

#include <fstream>

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

class EffectLoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;
public:
	virtual ~EffectLoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		this->manager = manager;

		compiler->registerLoader(this, "Effect Loader", "effect.xml");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		std::ifstream stream(fileName);

		std::string content = file::loadFromStream(stream);

		TiXmlDocument document;

		document.Parse(content.c_str());

		if(document.Error()) {
			throw Exception(document.ErrorDesc());
		}

		TiXmlElement* rootEffect = document.RootElement();
		if(strcmp("effect", rootEffect->Value()) != 0) {
			throw Exception("");
		}

		Effect* effect = new Effect(file::getFilename(fileName), manager);

		TiXmlElement* xmlSampler = rootEffect->FirstChildElement("sampler");
		while(xmlSampler) {

			xmlSampler = xmlSampler->NextSiblingElement("sampler");
		}

		TiXmlElement* xmlConstant = rootEffect->FirstChildElement("constant");
		while(xmlConstant) {

			xmlConstant = xmlConstant->NextSiblingElement("constant");
		}

		TiXmlElement* xmlVertexShader = rootEffect->FirstChildElement("vertexShader");
		TiXmlElement* xmlFragmentShader = rootEffect->FirstChildElement("fragmentShader");

		std::ifstream vs(xmlVertexShader->Attribute("src"));
		std::ifstream fs(xmlFragmentShader->Attribute("src"));

		Source* vertexShader = new Source(vs);
		Source* fragmentShader = new Source(fs);

		effect->setShader(new Shader(vertexShader, fragmentShader, 0));

		TiXmlElement* xmlAttribute = rootEffect->FirstChildElement("attribute");
		while(xmlAttribute) {
			const char* name = xmlAttribute->Attribute("name");
			const char* semmantic = xmlAttribute->Attribute("semmantic");

			if(name && semmantic)
				effect->setAttribute(name, semmantic);

			xmlAttribute = xmlAttribute->NextSiblingElement("attribute");
		}

		TiXmlElement* xmlColorWrite = rootEffect->FirstChildElement("colorWrite");
		if(xmlColorWrite)
			effect->enableColorWrite(strcmp(xmlColorWrite->Attribute("value"), "true") == 0);

		TiXmlElement* xmlDepthWrite = rootEffect->FirstChildElement("depthWrite");
		if(xmlDepthWrite)
			effect->enableDepthWrite(strcmp(xmlDepthWrite->Attribute("value"), "true") == 0);

		TiXmlElement* xmlBlend = rootEffect->FirstChildElement("blend");
		if(xmlBlend)
			effect->blendMode(0);

		TiXmlElement* xmlZFunc = rootEffect->FirstChildElement("zFunc");
		if(xmlZFunc)
			effect->depthTest(515);

		std::string outputName = file::getPath(fileName) + "/" + file::getFilename(fileName) + ".effect";
		FileStream fileStream(outputName);
		ResourceBinStream resourceStream(fileStream);
		EffectUtils::write(resourceStream, *manager, effect);

		delete effect;
	}
};

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new EffectLoader;
}
