/*
 * Scene.h
 *
 *  Created on: 09/05/2012
 *      Author: marrony
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "Resource.h"

#include <vector>
#include <string>
#include <map>

namespace engine {

	struct RenderQueueEntry {
		class ModelInstance* geometry;
		class Batch* batch;
		class Material* material;
	};

	class Scene : public Resource {
	private:
		std::vector<class Node*> nodes;

		std::vector<class Light*> pointLights;
		std::vector<class Light*> spotLights;
		std::vector<class ModelInstance*> models;
		std::vector<class Frustum*> cameras;

		class Frustum* atualCamera;
		class SceneTree* sceneTree;

		std::vector<RenderQueueEntry> renderQueue;

		class Node* root;
	public:
		static const Type TYPE;

		Scene(const std::string& name, ResourceManager* manager);
		virtual ~Scene();

		virtual Type getType() const {
			return TYPE;
		}

		std::vector<class ModelInstance*> getVisibleGeometries(const class Frustum* const camera);
		std::vector<class Light*> getVisibleLights(const class Frustum* const camera);

		void updateRenderQueue(class Frustum* camera);

		const std::vector<RenderQueueEntry>& getRenderQueue() {
			return renderQueue;
		}

		class Node* createNode();
		class Node* getRoot();

		class ModelInstance* createModelInstance(class Model* model, const class AABoundingBox& aabbox, Node* node = 0);

		class Light* createPointLight();
		class Light* createSpotLight();

		class Frustum* createCamera(const std::string& name);
		class Frustum* getActualCamera();

		class SceneTree* createSceneTree();

		void update(const class GameTime& gameTime, class ResourceManager& manager);

		friend class SceneUtils;
	};

	struct SceneUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	class SceneKey : public ResourceKey {
	public:
		SceneKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "scene/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

} /* namespace engine */

#endif /* SCENE_H_ */
