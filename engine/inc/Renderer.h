/*
 * Renderer.h
 *
 *  Created on: 07/03/2011
 *      Author: marrony
 */

#ifndef RENDERER_H_
#define RENDERER_H_

namespace engine {

	class GraphicManager;
	class Scene;

	struct Renderer {
		virtual ~Renderer() {}
		virtual void render(GraphicManager* graphicManager, Scene* sceneManager) = 0;
	};

}  // namespace engine

#endif /* RENDERER_H_ */
