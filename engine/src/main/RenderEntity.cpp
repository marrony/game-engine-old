/*
 * RenderEntity.cpp
 *
 *  Created on: 08/03/2011
 *      Author: marrony
 */

#include "RenderEntity.h"
#include "Geometry.h"
#include "Shader.h"
#include "GraphicManager.h"
#include "ResourceManager.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "Frustum.h"
#include "Light.h"

namespace engine {

	RenderEntity::RenderEntity(const std::string& aspect, ResourceManager& resourceManager) :
		actualAspect(aspect), resourceManager(resourceManager) {
	}

	void RenderEntity::render(GraphicManager* graphicManager, Geometry* geometry, Frustum* frustum, Light* light, float objectId) {
		ResourceId modelId = geometry->getModel();
		Model* model = (Model*)resourceManager.getResource(modelId);

		Buffer *indexBuffer = model->geometry->indexBuffer;
		Buffer *vertexBuffer = model->geometry->vertexBuffer;

		if(indexBuffer->isOk() && vertexBuffer->isOk()) {
			vertexBuffer->bind();
			indexBuffer->bind();

			for(size_t i = 0; i < model->getMeshCount(); i++) {
				Mesh* indexMesh = model->getIndexMesh(i);

				Material* material = (Material*)resourceManager.getResource(geometry->getMaterial(i));

				if(!material) break;

				ConstantContext context;

				context.graphicManager = graphicManager;
				context.resourceManager = &resourceManager;
				context.geometry = geometry;
				context.frustum = frustum;
				context.light = light;
				context.model = model;
				context.objectId = objectId;

				material->begin(actualAspect, context);

				graphicManager->drawIndex(indexBuffer, indexMesh->start, indexMesh->end, indexMesh->count, indexMesh->offset);

				material->end();
			}

			indexBuffer->unbind();
			vertexBuffer->unbind();
		}
	}

	void RenderEntity::renderScene(std::vector<Geometry*>& geometries, Frustum* frustum, GraphicManager* graphicManager, Light* light) {
		std::vector<Geometry*>::const_iterator ite;

		float objectId = 0;

		for(ite = geometries.begin(); ite != geometries.end(); ite++) {
			Geometry* geometry = *ite;

			render(graphicManager, geometry, frustum, light, objectId);

			objectId += 0.5;
		}
	}
}
