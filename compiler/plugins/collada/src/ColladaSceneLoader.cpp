/*
 * ColladaSceneLoader.cpp
 *
 *  Created on: 15/02/2011
 *      Author: marrony
 */
#include "ResourcePlugin.h"
#include "ResourceCompiler.h"
#include "Exception.h"
#include "Model.h"
#include "Material.h"
#include "FileUtil.h"

#include "tinyxml.h"

#include <fstream>
#include <iterator>

#include "ColladaDocument.h"
#include "CreateGeometry.h"
#include "CreateScene.h"

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

//////////////////////////////////////////////////////////////////

class ColladaEffectLambert : public ColladaEffectConstant {
public:
	static std::string elementType() {
		return "lambert";
	}

	virtual void loadFromXml(TiXmlElement* element) {
		ColladaElement::loadFromXml(element);
	}
};

//////////////////////////////////////////////////////////////////

class ColladaEffectBlinn : public ColladaEffectLambert {
public:
	static std::string elementType() {
		return "blinn";
	}

	virtual void loadFromXml(TiXmlElement* element) {
		ColladaElement::loadFromXml(element);
	}
};

//////////////////////////////////////////////////////////////////

class ColladaEffectPhong : public ColladaEffectBlinn {
public:
	static std::string elementType() {
		return "phong";
	}

	virtual void loadFromXml(TiXmlElement* element) {
		ColladaElement::loadFromXml(element);
	}
};

//////////////////////////////////////////////////////////////////

class ColladaLoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;

	void parse(const char* fileName, ColladaDocument& colladaDocument) {
		std::fstream sceneStream(fileName);
		std::string scene = file::loadFromStream(sceneStream);

		TiXmlDocument xmlDoc;
		xmlDoc.Parse(scene.c_str());

		if(xmlDoc.Error()) {
			throw Exception("COLLADA: Parse error");
		}

		TiXmlElement* root = xmlDoc.RootElement();

		if(root->Value() != std::string("COLLADA")) {
			throw Exception("Not a COLLADA file");
		}

		if(root->Attribute("version") != std::string("1.4.1")) {
			throw Exception("Invalid version");
		}

		colladaDocument.loadFromXml(root);
	}

	void saveModel(const std::string& path, Model* model) {
		std::string outputName = path + "/" + model->getName() + ".model";
		FileStream fileStream(outputName);
		ResourceBinStream resourceStream(fileStream);
		ModelUtils::write(resourceStream, *manager, model);
	}
public:
	virtual ~ColladaLoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		this->manager = manager;

		compiler->registerLoader(this, "Collada Loader", "dae");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		try {
			ColladaDocument colladaDocument;

			parse(fileName, colladaDocument);

			if(options["only-geometry"] == "true") {
				for(ColladaGeometry* geometry : colladaDocument.libraryGeometries->getItems()) {
					CreateGeometry createGeometry(options["geometry-name"], manager);

					geometry->accept(&createGeometry);

					saveModel(file::getPath(fileName), createGeometry.getModel());
				}
			} else {
				CreateScene createScene(compiler, manager, this, &colladaDocument);
				colladaDocument.scene->accept(&createScene);

				for(Resource* resource : createScene.getResourcesCreated()) {
					if(Model* model = dynamic_cast<Model*>(resource)) {
						saveModel(file::getPath(fileName), model);
					}
				}
			}
		} catch(...) {
			throw;
		}
	}

	virtual void destroyResource(engine::Resource* resource) {
	}
};

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new ColladaLoader;
}
