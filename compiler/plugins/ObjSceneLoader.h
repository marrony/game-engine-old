/*
 * ObjSceneLoader.h
 *
 *  Created on: 03/02/2011
 *      Author: marrony
 */
#ifndef OBJSCENELOADER_H_
#define OBJSCENELOADER_H_

#include "ModelLoader.h"

namespace engine {

	class SceneManager;
	class ResourceManager;
	class GraphicManager;

	class ObjModelLoader : public ModelLoader {
	public:
		ObjModelLoader(SceneManager* sceneManager, ResourceManager* resourceManager, GraphicManager& graphicManager);

		virtual Model* loadModel(std::istream& file);

	private:
		SceneManager* sceneManager;
		ResourceManager* resourceManager;
		GraphicManager& graphicManager;
	};

}  // namespace engine

#endif /* OBJSCENELOADER_H_ */
