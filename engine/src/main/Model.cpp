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
		for(Batch& mesh : batches) {
			manager->unloadResource(mesh.material);
		}

		delete vertexBuffer;
		delete indexBuffer;
	}

	Resource* ModelKey::loadResource(class ResourceManager& manager) const {
		std::string modelName = getName();

		FileStream fileStream("resources/scene/" + modelName + ".model");
		ResourceBinStream resourceStream(fileStream);

		ModelBuilder builder(manager);
		return builder.readFromStream(resourceStream);
	}

} /* namespace engine */
