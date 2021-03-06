/*
 * SimpleRenderer.h
 *
 *  Created on: 24/11/2010
 *      Author: marrony.neris
 */

#ifndef SIMPLERENDERER_H_
#define SIMPLERENDERER_H_

#include "Shader.h"
#include "MyImpls.h"
#include "Texture.h"
#include "Geometry.h"
#include "ResourceManager.h"

#include <string>
#include <fstream>
#include <iostream>

using namespace engine;

class SimpleRenderer : public Renderer {

	void renderScene(Frustum* frustum, GraphicManager* graphicManager, Light* light, const std::string& aspect, Scene* scene) {
		for(RenderQueueEntry entry : scene->getRenderQueue()) {
			Model* model = entry.geometry->getModel();

			Buffer* indexBuffer = 0;
			Buffer* vertexBuffer = 0;

			graphicManager->getBuffers(model->geometryData, &indexBuffer, &vertexBuffer);

			if(!indexBuffer || !vertexBuffer)
				continue;

			graphicManager->setVertexBuffer(vertexBuffer);
			graphicManager->setIndexBuffer(indexBuffer);

			ConstantContext context;

			context.graphicManager = graphicManager;
			context.geometry = entry.geometry;
			context.frustum = frustum;
			context.light = light;
			context.model = model;
			context.objectId = 0;

			entry.material->begin(aspect, context);

			graphicManager->drawIndex(entry.batch->start, entry.batch->end, entry.batch->count, entry.batch->offset);
		}
	}

public:
	virtual void render(GraphicManager* graphicManager, Scene* sceneManager) {
		Frustum* camera = sceneManager->getActualCamera();

		sceneManager->updateRenderQueue(camera);

		graphicManager->clear(math::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0);

		graphicManager->disableBlend();
		graphicManager->disableColorWrite();
		graphicManager->depthTest(GL_LEQUAL);

		renderScene(camera, graphicManager, 0, "pass0", sceneManager);

		graphicManager->enableColorWrite();
		graphicManager->disableDepthWrite();
		graphicManager->enableAditiveBlend();

		for(Light* light : sceneManager->getVisibleLights(camera)) {
			renderScene(camera, graphicManager, light, "pass0", sceneManager);
		}
	}
};

#endif /* SIMPLERENDERER_H_ */
