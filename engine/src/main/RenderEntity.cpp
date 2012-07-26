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
	}

	void RenderEntity::renderScene(std::vector<Geometry*>& geometries, Frustum* frustum, GraphicManager* graphicManager, Light* light) {
	}
}
