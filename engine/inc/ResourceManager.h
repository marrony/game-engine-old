/*
 * ResourceManager.h
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include "Resource.h"
#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "Scene.h"

#include <map>
#include <vector>
#include <string>
#include <functional>

namespace engine {

	class ResourceManager {
		std::vector<ResourceListener*> listeners;

		template<typename T>
		struct ResourceEntry {
			T* resource;
			int count;
		};

		std::map<std::string, ResourceEntry<Scene>> scenes;
		std::map<std::string, ResourceEntry<Texture>> textures;
		std::map<std::string, ResourceEntry<Material>> materials;
		std::map<std::string, ResourceEntry<Effect>> effects;
		std::map<std::string, ResourceEntry<Shader>> shaders;
		std::map<std::string, ResourceEntry<Model>> models;

		template<typename T>
		T* loadResource(const std::string& resourceName, std::map<std::string, ResourceEntry<T>>& resources, const std::function<T*(const std::string&)>& callback);

		template<typename T>
		void unloadResource(Resource* resource, std::map<std::string, ResourceEntry<T>>& resources);
	public:
		ResourceManager();
		~ResourceManager();

		Scene* loadScene(const std::string& sceneName);
		void unloadScene(Scene* scene);

		Texture* loadTexture(const std::string& textureName);
		void unloadTexture(Texture* texture);

		Material* loadMaterial(const std::string& materialName);
		void unloadMaterial(Material* material);

		Effect* loadEffect(const std::string& effectName);
		void unloadEffect(Effect* effect);

		Shader* loadShader(const std::string& shaderName);
		void unloadShader(Shader* shader);

		Model* loadModel(const std::string& modelName);
		void unloadModel(Model* model);

		void addListener(ResourceListener* listener);
		void removeListener(ResourceListener* listener);
	};

} /* namespace engine */

#endif /* RESOURCEMANAGER_H_ */
