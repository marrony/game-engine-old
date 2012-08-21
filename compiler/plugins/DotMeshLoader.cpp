/*
 * DotMeshLoader.cpp
 *
 *  Created on: 30/03/2012
 *      Author: marrony.neris
 */

#include "ResourcePlugin.h"
#include "ResourceCompiler.h"

#include "Resource.h"
#include "GeometryData.h"

#include "SceneManager.h"
#include "Model.h"
#include "Exception.h"
#include "FileUtil.h"
#include "tinyxml.h"
#include "Stream.h"

#include <string>
#include <vector>

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

class DotMeshLoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;
public:
	virtual ~DotMeshLoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		this->manager = manager;

		compiler->registerLoader(this, "Dot Mesh Loader", "mesh.xml");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		TiXmlDocument xmlDoc;
		xmlDoc.LoadFile(fileName);

		if(xmlDoc.Error()) {
			throw Exception("Parse error");
		}

		TiXmlElement *xmlMesh = xmlDoc.RootElement();
		if(std::string(xmlMesh->Value()) != "mesh") {
			throw Exception("Tag mesh not found");
		}

		GeometryData* modelData = new GeometryData;

		TiXmlElement* xmlSubmeshes = xmlMesh->FirstChildElement("submeshes");
		if(xmlSubmeshes != 0) {
			processSubmeshes(modelData, xmlSubmeshes);
		}

		std::string outputName = file::getPath(fileName) + "/" + file::getFilename(fileName) + ".model";
		FileStream fileStream(outputName);
		ResourceBinStream resourceStream(fileStream);

		modelData->writeToStream(resourceStream);
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

	void processSubmeshes(GeometryData* modelData, TiXmlElement* xmlSubmeshes) {
		TiXmlElement* xmlSubmesh = xmlSubmeshes->FirstChildElement("submesh");

		while(xmlSubmesh != 0) {
			processSubmesh(modelData, xmlSubmesh);
			xmlSubmesh = xmlSubmesh->NextSiblingElement("submesh");
		}
	}

	void processSubmesh(GeometryData* modelData, TiXmlElement* xmlSubmesh) {
		std::vector<unsigned short> indices;
		std::vector<MeshVertex> vertices;
		int flags = 0;

		TiXmlElement* xmlFaces = xmlSubmesh->FirstChildElement("faces");
		if(xmlFaces != 0) {
			indices = processFaces(modelData, xmlFaces);
		}

		TiXmlElement* xmlGeometry = xmlSubmesh->FirstChildElement("geometry");
		if(xmlGeometry != 0) {
			vertices = processGeometry(modelData, xmlGeometry, flags);
		}

		std::string material = xmlSubmesh->Attribute("material");

		//TODO carregar
		modelData->addVertexData(vertices, indices, 0 /*material*/, flags);
	}

	std::vector<unsigned short> processFaces(GeometryData* modelData, TiXmlElement* xmlFaces) {
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

	std::vector<MeshVertex> processGeometry(GeometryData* modelData, TiXmlElement* xmlGeometry, int& flags) {
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
