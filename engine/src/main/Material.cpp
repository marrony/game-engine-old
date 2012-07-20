/*
 * Material.cpp
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#include <fstream>

#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "SceneManager.h"
#include "GraphicManager.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "FileUtil.h"
#include "tinyxml.h"
#include "Exception.h"
#include "Model.h"
#include "Frustum.h"
#include "Light.h"
#include "Geometry.h"

#include "MemoryManager.h"

namespace engine {

	const Type Material::TYPE("material");

	Material::Material(const std::string& name, Effect* effect) :
			Resource(name), effect(effect) {
	}

	Material::~Material() {
		manager->unloadResource(effect);

		for(auto sampler : samplers)
			manager->unloadResource(sampler.second);
	}

	void Material::addSampler(const std::string& samplerName, Texture* sampler) {
		samplers[samplerName] = sampler;
	}

	Texture* Material::getSampler(const std::string& samplerName) {
		return samplers[samplerName];
	}

	void Material::begin(const std::string& aspectName, ConstantContext& context) {
		effect->begin(context);

		for(auto sampler : samplers) {
			context.graphicManager->bindTexture(sampler.first, sampler.second);
		}
	}

	void Material::end() {
//		aspect->end(graphicManager);
//
//		graphicManager->unbindShader();
	}



	void* MaterialUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		stream.pushGroup("material");

		std::string type = stream.readString("type");
		std::string name = stream.readString("name");

		if(type != "material")
			throw Exception("Resource is not a material");

		Material* material = new Material(type + "/" + name, 0);
		material->manager = &manager;

		std::string effectName = stream.readString("effectName");
		material->effect = (Effect*) manager.loadResource(EffectKey(effectName));

		int texCount = stream.readInt("textureCount");
		for(int i = 0; i < texCount; i++) {
			stream.pushGroup("texture");

			std::string textureName = stream.readString("textureName");
			std::string resourceName = stream.readString("resourceName");

			stream.popGroup();

			Texture* texture = (Texture*)manager.loadResource(TextureKey(resourceName));
			material->samplers.insert(std::make_pair(textureName, texture));
		}

		return material;
	}

	void MaterialUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Material* material = dynamic_cast<Material*>((Material*)instance);

		if(!material)
			throw Exception("Resource is not a material");

		stream.pushGroup("material");

		stream.writeString("type", material->getType().getName());
		stream.writeString("name", material->name);
		stream.writeString("effectName", material->effect->getName());

		stream.writeInt("textureCount", material->samplers.size());

		for(auto texture : material->samplers) {
			stream.pushGroup("texture");

			stream.writeString("textureName", texture.first);
			stream.writeString("resourceName", texture.second->getName());

			stream.popGroup();
		}
	}

	Resource* MaterialKey::loadResource(ResourceManager& manager) const {
		std::string materialName = getName();

		FileStream fileStream("resources/materials/" + materialName + ".material");
		ResourceBinStream resourceStream(fileStream);

		Material* material = (Material*)MaterialUtils::read(resourceStream, manager, 0);

		MaterialEvent event;
		event.type = "material";
		event.material = material;

		manager.dispatchLoadedEvent(event);

		return material;
	}

}  // namespace engine
