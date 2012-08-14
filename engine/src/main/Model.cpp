/*
 * Model.cpp
 *
 *  Created on: 13/07/2011
 *      Author: marrony
 */

#include "Model.h"
#include "Exception.h"
#include "ResourceManager.h"
#include "GraphicManager.h"
#include "Stream.h"
#include "ModelBuilder.h"

#include "MemoryManager.h"

namespace engine {
	const Type Model::TYPE("model");

	Model::Model(const std::string& name, ResourceManager* manager) :
			Resource(name, manager), vertexBuffer(0), indexBuffer(0) {
	}

	Model::~Model() {
		for(size_t i = 0; i < batches.size(); ++i) {
			Material* material = materials[i];
			manager->unloadResource(material);
		}

		delete vertexBuffer;
		delete indexBuffer;
	}

	void Model::load(const std::string& filename) {
		FileStream fileStream("resources/scene/" + filename + ".model");
		ResourceBinStream resourceStream(fileStream);

		ModelBuilder builder(*manager);
		builder.readFromStream(resourceStream);
		builder.createModel(this);
	}

	Resource* ModelKey::loadResource(class ResourceManager& manager) const {
		std::string modelName = getName();

		FileStream fileStream("resources/scene/" + modelName + ".model");
		ResourceBinStream resourceStream(fileStream);

		ModelBuilder builder(manager);
		builder.readFromStream(resourceStream);
		return builder.createModel();
	}

} /* namespace engine */
