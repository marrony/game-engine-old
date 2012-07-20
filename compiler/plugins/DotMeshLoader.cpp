/*
 * DotMeshLoader.cpp
 *
 *  Created on: 30/03/2012
 *      Author: marrony.neris
 */

#include "ResourcePlugin.h"
#include "ResourceCompiler.h"

#include "Resource.h"
#include "Model.h"

#include "SceneManager.h"
#include "Model.h"
#include "Exception.h"
#include "FileUtil.h"
#include "tinyxml.h"

#include <fstream>
#include <string>
#include <vector>

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

class DotMeshLoader : public ResourceLoader {
	ResourceCompiler* compiler;
public:
	virtual ~DotMeshLoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		compiler->registerLoader(this, "Dot Mesh Loader", "mesh.xml");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		TiXmlDocument xmlDoc;

		std::ifstream meshStream(fileName);
		std::string data = file::loadFromStream(meshStream);
		xmlDoc.Parse(data.c_str());

		if(xmlDoc.Error()) {
			throw Exception("Parse error");
		}

		TiXmlElement *xmlMesh = xmlDoc.RootElement();
		if(std::string(xmlMesh->Value()) != "mesh") {
			throw Exception("Tag mesh not found");
		}

		Model* model = new Model(file::getFilename(fileName));

		TiXmlElement* xmlSubmeshes = xmlMesh->FirstChildElement("submeshes");
		if(xmlSubmeshes != 0) {
			processSubmeshes(model, xmlSubmeshes);
		}

		model->modelData->calculateTangent();
		model->modelData->calculateBoundingBox();
	}

	bool getAttribute(TiXmlElement* xmlElement, const char* attribute, bool valueDefault) {
		const char* valueStr = xmlElement->Attribute(attribute);

		if(valueStr == 0) {
			return valueDefault;
		}

		return std::string(valueStr) == "true";
	}

	float getAttribute(TiXmlElement* xmlElement, const char* attribute, float valueDefault) {
		const char* valueStr = xmlElement->Attribute(attribute);

		if(valueStr == 0) {
			return valueDefault;
		}

		float value;
		sscanf(valueStr, "%f", &value);
		return value;
	}

	int getAttribute(TiXmlElement* xmlElement, const char* attribute, int valueDefault) {
		const char* valueStr = xmlElement->Attribute(attribute);

		if(valueStr == 0) {
			return valueDefault;
		}

		int value;
		sscanf(valueStr, "%d", &value);
		return value;
	}

	void processSubmeshes(Model* model, TiXmlElement* xmlSubmeshes) {
		TiXmlElement* xmlSubmesh = xmlSubmeshes->FirstChildElement("submesh");

		while(xmlSubmesh != 0) {
			processSubmesh(model, xmlSubmesh);
			xmlSubmesh = xmlSubmesh->NextSiblingElement("submesh");
		}
	}

	void processSubmesh(Model* model, TiXmlElement* xmlSubmesh) {
		std::vector<unsigned short> indices;
		std::vector<MeshVertex> vertices;
		int flags = 0;

		TiXmlElement* xmlFaces = xmlSubmesh->FirstChildElement("faces");
		if(xmlFaces != 0) {
			indices = processFaces(model, xmlFaces);
		}

		TiXmlElement* xmlGeometry = xmlSubmesh->FirstChildElement("geometry");
		if(xmlGeometry != 0) {
			vertices = processGeometry(model, xmlGeometry, flags);
		}

		std::string material = xmlSubmesh->Attribute("material");

		//TODO carregar
		model->addVertexData(vertices, indices, 0 /*material*/, flags);
	}

	std::vector<unsigned short> processFaces(Model* model, TiXmlElement* xmlFaces) {
		std::vector<unsigned short> indices;

		TiXmlElement* xmlFace = xmlFaces->FirstChildElement("face");
		while(xmlFace != 0) {
			unsigned int v1, v2, v3;

			sscanf(xmlFace->Attribute("v1"), "%u", &v1);
			sscanf(xmlFace->Attribute("v2"), "%u", &v2);
			sscanf(xmlFace->Attribute("v3"), "%u", &v3);

			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);

			xmlFace = xmlFace->NextSiblingElement("face");
		}

		return indices;
	}

	std::vector<MeshVertex> processGeometry(Model* model, TiXmlElement* xmlGeometry, int& flags) {
		std::vector<MeshVertex> vertices;

		TiXmlElement* xmlVertexBuffer = xmlGeometry->FirstChildElement("vertexbuffer");

		if(xmlVertexBuffer == 0) {
			return vertices;
		}

		bool parsePosition = getAttribute(xmlVertexBuffer, "positions", false);
		bool parseNormal = getAttribute(xmlVertexBuffer, "normals", false);
		int texCoords = getAttribute(xmlVertexBuffer, "texture_coords", 0);

		TiXmlElement* xmlVertex = xmlVertexBuffer->FirstChildElement("vertex");
		while(xmlVertex != 0) {
			MeshVertex vertex;

			if(parsePosition) {
				TiXmlElement* xmlPosition = xmlVertex->FirstChildElement("position");

				vertex.position.x = getAttribute(xmlPosition, "x", 0.0f);
				vertex.position.y = getAttribute(xmlPosition, "y", 0.0f);
				vertex.position.z = getAttribute(xmlPosition, "z", 0.0f);

				flags |= MeshVertex::POSITION;
			}

			if(parseNormal) {
				TiXmlElement* xmlNormal = xmlVertex->FirstChildElement("normal");

				vertex.normal.x = getAttribute(xmlNormal, "x", 0.0f);
				vertex.normal.y = getAttribute(xmlNormal, "y", 0.0f);
				vertex.normal.z = getAttribute(xmlNormal, "z", 0.0f);

				flags |= MeshVertex::NORMAL;
			}

			for(int i = 0; i < texCoords; i++) {
				TiXmlElement* xmlTexCoords = xmlVertex->FirstChildElement("texcoord");

				vertex.texCoord.x = getAttribute(xmlTexCoords, "u", 0.0f);
				vertex.texCoord.y = getAttribute(xmlTexCoords, "v", 0.0f);

				flags |= MeshVertex::TEXTURE;

				//TODO colocar mais texCoords, por enquanto so a primeira
				break;
			}

			vertices.push_back(vertex);

			xmlVertex = xmlVertex->NextSiblingElement("vertex");
		}

		return vertices;
	}
};

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new DotMeshLoader;
}
