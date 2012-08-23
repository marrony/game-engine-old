/*
 * RenderEntity.h
 *
 *  Created on: 08/03/2011
 *      Author: marrony
 */

#ifndef RENDERENTITY_H_
#define RENDERENTITY_H_

#include <string>
#include <vector>

namespace engine {

	class ModelInstance;
	class GraphicManager;
	class ResourceManager;
	class Attribute;
	class Constant;
	class Shader;
	class Frustum;
	class Light;
	class Material;

	//TODO remover essa classe
	class RenderEntity {
	public:
		RenderEntity(const std::string& aspect, ResourceManager& resourceManager);

		void render(GraphicManager* graphicManager, ModelInstance* geometry, Frustum* frustum, Light* light, float objectId);

		void renderScene(std::vector<ModelInstance*>& geometries, Frustum* frustum, GraphicManager* graphicManager, Light* light);
	private:
		std::string actualAspect;
		ResourceManager& resourceManager;
	};

}

#endif /* RENDERENTITY_H_ */
