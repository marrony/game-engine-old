/*
 * ResourceManager.cpp
 *
 *  Created on: 18/03/2012
 *      Author: Marrony
 */

#include "ResourceManager.h"
#include "GeneralHashFunction.h"
#include "Exception.h"
#include "FileUtil.h"
#include "Image.h"
#include "GraphicManager.h"

#include <fstream>
#include <string.h>

namespace engine {

	ResourceManager::ResourceManager() {
	}

	ResourceManager::~ResourceManager() {
	}

	Scene* ResourceManager::loadScene(const std::string& sceneName) {
		FileStream fileStream("resources/scene/" + sceneName + ".scene");
		ResourceBinStream resourceStream(fileStream);
		Scene* scene = (Scene*)SceneUtils::read(resourceStream, *this, 0);

		return scene;
	}

	void ResourceManager::unloadScene(Scene* scene) {
		unloadResource(scene, scenes);
	}

	Texture* ResourceManager::loadTexture(const std::string& textureName) {
		Texture* texture = new Texture(textureName);

		FileStream fileStream("resources/images/" + textureName + ".texture");
		ResourceBinStream resourceStream(fileStream);

		Image* image = (Image*)TextureUtils::read(resourceStream, *this, 0);

		for(auto listener : listeners)
			listener->onTexture(texture, image);

		delete image;

		return texture;
	}

	void ResourceManager::unloadTexture(Texture* texture) {
		unloadResource(texture, textures);
	}

	Material* ResourceManager::loadMaterial(const std::string& materialName) {
		auto entry = materials.find(materialName);

		Material* material;

		if(entry != materials.end())
			material = entry->second.resource;
		else {
			FileStream fileStream("resources/materials/" + materialName + ".material");
			ResourceBinStream resourceStream(fileStream);

			material = (Material*)MaterialUtils::read(resourceStream, *this, 0);

			for(auto listener : listeners)
				listener->onMaterial(material);

			entry = materials.insert({materialName, {material, 0}}).first;
		}

		entry->second.count++;

		return material;
	}

	void ResourceManager::unloadMaterial(Material* material) {
		unloadResource(material, materials);
	}

	Effect* ResourceManager::loadEffect(const std::string& effectName) {
		FileStream fileStream("resources/materials/" + effectName + ".effect");
		ResourceBinStream resourceStream(fileStream);
		Effect* effect = (Effect*)EffectUtils::read(resourceStream, *this, 0);

		for(auto listener : listeners)
			listener->onEffect(effect);

		return effect;
	}

	void ResourceManager::unloadEffect(Effect* effect) {
		unloadResource(effect, effects);
	}

	Shader* ResourceManager::loadShader(const std::string& shaderName) {
		return 0;
	}

	void ResourceManager::unloadShader(Shader* shader) {
		//unloadResource(shader, shaders);
	}

	Model* ResourceManager::loadModel(const std::string& modelName) {
		FileStream fileStream("resources/scene/" + modelName + ".model");
		ResourceBinStream resourceStream(fileStream);
		Model* model = (Model*) ModelUtils::read(resourceStream, *this, 0);

		for(auto listener : listeners)
			listener->onModel(model);

		return model;
	}

	void ResourceManager::unloadModel(Model* model) {
		unloadResource(model, models);
	}

	void ResourceManager::addListener(ResourceListener* listener) {
		listeners.push_back(listener);
	}

	void ResourceManager::removeListener(ResourceListener* listener) {
	}

	void ResourceManager::unloadResource(Resource* resource, std::map<std::string, ResourceEntry<Material>>& resources) {
		std::string resourceName = resource->getName();

		auto entry = resources.find(resourceName);

		if(entry == resources.end()) return;

		if(--entry->second.count > 0) return;

		resources.erase(entry);

		delete entry->second.resource;
	}

} /* namespace engine */
