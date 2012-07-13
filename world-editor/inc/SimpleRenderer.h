/*
 * SimpleRenderer.h
 *
 *  Created on: 24/11/2010
 *      Author: marrony.neris
 */

#ifndef SIMPLERENDERER_H_
#define SIMPLERENDERER_H_

#include "win32.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "RenderEntity.h"
#include "Geometry.h"
#include "Light.h"
#include "Frustum.h"
#include "Scene.h"
#include "GameEntity.h"

#include <string>
#include <fstream>

class SimpleRenderer : public engine::Renderer {

public:
	virtual void render(engine::GraphicManager* graphicManager, engine::Scene* sceneManager) {
	}
};

#endif /* SIMPLERENDERER_H_ */
