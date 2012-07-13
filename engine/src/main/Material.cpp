/*
 * Material.cpp
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#include <fstream>

#include "Material.h"
#include "MaterialAttribute.h"
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

	const Type Effect::TYPE("effect");
	const Type Material::TYPE("material");

	void Effect::enableDepthWrite(bool depthWriteEnabled) {
		usingDephtWrite = true;
		this->depthWriteEnabled = depthWriteEnabled;
	}

	void Effect::enableColorWrite(bool colorWriteEnabled) {
		usingColorWrite = true;
		this->colorWriteEnabled = colorWriteEnabled;
	}

	void Effect::depthTest(int depthFunc) {
		usingDepthTest = true;
		this->depthFunc = depthFunc;
	}

	void Effect::blendMode(int blendMode) {
		usingBlend = true;
		this->_blendMode = blendMode;
	}

	void Effect::cullMode(int cullMode) {
		usingCull = true;
		this->_cullMode = cullMode;
	}

	void Effect::begin(ConstantContext& context) {
		context.graphicManager->setShader(shader);

		if(usingDephtWrite) {
			if(depthWriteEnabled)
				context.graphicManager->enableDepthWrite();
			else
				context.graphicManager->disableDepthWrite();
		}

		if(usingColorWrite) {
			if(colorWriteEnabled)
				context.graphicManager->enableColorWrite();
			else
				context.graphicManager->disableColorWrite();
		}

		if(usingDepthTest) {
			context.graphicManager->depthTest(depthFunc);
		}

		if(usingBlend) {
			context.graphicManager->enableAditiveBlend();
		}

		if(usingCull) {
		}

		setupConstants(context);
		setupAttributes(context.model, shader, context.graphicManager);
	}

	void Effect::end(ConstantContext& context) {
		if(usingDephtWrite) {
		}

		if(usingColorWrite) {
		}

		if(usingDepthTest) {
		}

		if(usingBlend) {
			context.graphicManager->disableBlend();
		}

		if(usingCull) {
		}
	}

	void Effect::setAttribute(const std::string& name, const std::string& semmantic) {
		if(semmantic == "POSITION") {
			attributesEnabled[AttributeOffset::PositionOffset].enabled = true;
			attributesEnabled[AttributeOffset::PositionOffset].name = name;
			attributesEnabled[AttributeOffset::PositionOffset].mode = 3;
		}

		if(semmantic == "NORMAL") {
			attributesEnabled[AttributeOffset::NormalOffset].enabled = true;
			attributesEnabled[AttributeOffset::NormalOffset].name = name;
			attributesEnabled[AttributeOffset::NormalOffset].mode = 3;
		}

		if(semmantic == "STANGENT") {
			attributesEnabled[AttributeOffset::STangentOffset].enabled = true;
			attributesEnabled[AttributeOffset::STangentOffset].name = name;
			attributesEnabled[AttributeOffset::STangentOffset].mode = 3;
		}

		if(semmantic == "TTANGENT") {
			attributesEnabled[AttributeOffset::TTangentOffset].enabled = true;
			attributesEnabled[AttributeOffset::TTangentOffset].name = name;
			attributesEnabled[AttributeOffset::TTangentOffset].mode = 3;
		}

		if(semmantic == "COLOR") {
			attributesEnabled[AttributeOffset::ColorOffset].enabled = true;
			attributesEnabled[AttributeOffset::ColorOffset].name = name;
			attributesEnabled[AttributeOffset::ColorOffset].mode = 3;
		}

		if(semmantic == "TEXCOORD") {
			attributesEnabled[AttributeOffset::TexCoordOffset].enabled = true;
			attributesEnabled[AttributeOffset::TexCoordOffset].name = name;
			attributesEnabled[AttributeOffset::TexCoordOffset].mode = 2;
		}

		if(semmantic == "BONEID") {
			attributesEnabled[AttributeOffset::BoneIdsOffset].enabled = true;
			attributesEnabled[AttributeOffset::BoneIdsOffset].name = name;
			attributesEnabled[AttributeOffset::BoneIdsOffset].mode = 4;
		}

		if(semmantic == "WEIGHT") {
			attributesEnabled[AttributeOffset::WeigthsOffset].enabled = true;
			attributesEnabled[AttributeOffset::WeigthsOffset].name = name;
			attributesEnabled[AttributeOffset::WeigthsOffset].mode = 4;
		}
	}

	void Effect::setupConstants(ConstantContext& context) {
		const math::Matrix4 modelViewMatrix = context.frustum->getViewMatrix() * context.geometry->getTransformation();
		const math::Matrix3 normalMatrix = modelViewMatrix.normalMatrix();

		for(ConstantsEnabled c : constants) {
			Constant* cc = shader->getConstant(c.name);

			switch(c.constant) {
			case Constants::ZNear:
				cc->setValue(context.frustum->getNear());
				break;

			case Constants::ZFar:
				cc->setValue(context.frustum->getFar());
				break;

			case Constants::ProjectionMatrix:
				cc->setValue(context.frustum->getProjectionMatrix());
				break;

			case Constants::ModelViewMatrix:
				cc->setValue(modelViewMatrix);
				break;

			case Constants::NormalMatrix:
				cc->setValue(normalMatrix);
				break;

			case Constants::LightPosition:
				if(context.light) {
					cc->setValue(context.frustum->getViewMatrix() * context.light->getPosition());
				} else {
					cc->setValue(context.frustum->getViewMatrix() * math::Vector3(0, 20, 0));
				}
				break;

			case Constants::LightColor:
				if(context.light) {
					cc->setValue(context.light->getDiffuse());
				} else {
					cc->setValue(math::Vector3(1, 1, 1));
				}
				break;

			case Constants::ObjectId:
				cc->setValue(context.objectId);
				break;

			case Constants::BonePallete:
				size_t bonesCount = context.model->getAnimation().getBonesCount();

				if(!context.model->geometry->boneIds.empty() && !context.model->geometry->weights.empty() && bonesCount > 0) {
					cc->setValue(context.geometry->getBoneMatrix(), bonesCount);
				} else {
					cc->setValue(&math::Matrix4::IDENTITY, 1);
				}

				break;
			}
		}
	}

	void Effect::setupAttributes(Model* model, Shader* shader, GraphicManager* graphicManager) {
		Geo* geometry = model->geometry;

		for(int i = 0; i < AttributeOffset::MaxAttributeOffset; i++) {
			if(!attributesEnabled[i].enabled) continue;

			Attribute* attrib = shader->getAttribute(attributesEnabled[i].name);

			graphicManager->setAttribute(i, attrib, attributesEnabled[i].mode, geometry->attributeOffsets[i], sizeof(float) * geometry->elementsPerVertex);
		}
	}

	Material::Material(const std::string& name, ResourceId effectId) :
			Resource(name), effectId(effectId) {
	}

	Material::~Material() {
		//delete effect;
	}

	void Material::addSampler(const std::string& samplerName, ResourceId sampler) {
		samplers[samplerName] = sampler;
	}

	ResourceId Material::getSampler(const std::string& samplerName) {
		return samplers[samplerName];
	}

	void Material::begin(const std::string& aspectName, ConstantContext& context) {
		Effect* effect = (Effect*)manager->getResource(effectId);

		effect->begin(context);

		for(auto sampler : samplers) {
			Texture* texture = (Texture*)manager->getResource(sampler.second);

			if(texture)
				context.graphicManager->bindTexture(sampler.first, texture);
		}
	}

	void Material::end() {
//		aspect->end(graphicManager);
//
//		graphicManager->unbindShader();
	}

	Effect::~Effect() {
	}

	void* EffectUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		stream.pushGroup("effect");

		std::string type = stream.readString("type");
		std::string name = stream.readString("name");

		std::string vs = stream.readString("vertexShader");
		std::string fs = stream.readString("fragmentShader");

		Source* vss = new Source(vs);
		Source* fss = new Source(fs);

		Effect* effect = new Effect(name);

		effect->manager = &manager;
		effect->shader = new Shader(vss, fss, 0);
		effect->shader->link();

		effect->usingDephtWrite = stream.readByte("usingDephtWrite");
		effect->usingColorWrite = stream.readByte("usingColorWrite");
		effect->usingDepthTest = stream.readByte("usingDepthTest");
		effect->usingBlend = stream.readByte("usingBlend");
		effect->usingCull = stream.readByte("usingCull");

		effect->depthWriteEnabled = stream.readByte("depthWriteEnabled");
		effect->colorWriteEnabled = stream.readByte("colorWriteEnabled");
		effect->depthFunc = stream.readInt("depthFunc");
		effect->_blendMode = stream.readInt("blendMode");
		effect->_cullMode = stream.readInt("cullMode");

		for(int i = 0; i < AttributeOffset::MaxAttributeOffset; i++) {
			effect->attributesEnabled[i].name = stream.readString("");
			effect->attributesEnabled[i].enabled = stream.readByte("") != 0;
			effect->attributesEnabled[i].mode = stream.readByte("");
		}

		effect->constants.push_back(Effect::ConstantsEnabled("myZNear", Constants::ZNear));
		effect->constants.push_back(Effect::ConstantsEnabled("myZFar", Constants::ZFar));
		effect->constants.push_back(Effect::ConstantsEnabled("myProjectionMatrix", Constants::ProjectionMatrix));
		effect->constants.push_back(Effect::ConstantsEnabled("myModelViewMatrix", Constants::ModelViewMatrix));
		effect->constants.push_back(Effect::ConstantsEnabled("myNormalMatrix", Constants::NormalMatrix));
		effect->constants.push_back(Effect::ConstantsEnabled("myLightPosition", Constants::LightPosition));
		effect->constants.push_back(Effect::ConstantsEnabled("myLightColor", Constants::LightColor));
		effect->constants.push_back(Effect::ConstantsEnabled("myObjectId", Constants::ObjectId));
		effect->constants.push_back(Effect::ConstantsEnabled("myBonePalete", Constants::BonePallete));

		stream.popGroup();

		return effect;
	}

	void EffectUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Effect* effect = reinterpret_cast<Effect*>((Effect*) instance);

		if(!effect)
			throw Exception("Resource is not a effect");

		stream.pushGroup("effect");
		stream.writeString("type", effect->getType().getName());
		stream.writeString("name", effect->name);

		stream.writeString("vertexShader", effect->shader->getVertexShader()->getSource());
		stream.writeString("fragmentShader", effect->shader->getFragmentShader()->getSource());

		stream.writeByte("usingDephtWrite", effect->usingDephtWrite);
		stream.writeByte("usingColorWrite", effect->usingColorWrite);
		stream.writeByte("usingDepthTest", effect->usingDepthTest);
		stream.writeByte("usingBlend", effect->usingBlend);
		stream.writeByte("usingCull", effect->usingCull);

		stream.writeByte("depthWriteEnabled", effect->depthWriteEnabled);
		stream.writeByte("colorWriteEnabled", effect->colorWriteEnabled);
		stream.writeInt("depthFunc", effect->depthFunc);
		stream.writeInt("blendMode", effect->_blendMode);
		stream.writeInt("cullMode", effect->_cullMode);

		for(int i = 0; i < AttributeOffset::MaxAttributeOffset; i++) {
			stream.writeString("", effect->attributesEnabled[i].name);
			stream.writeByte("", effect->attributesEnabled[i].enabled);
			stream.writeByte("", effect->attributesEnabled[i].mode);
		}

		stream.popGroup();
	}

	void* MaterialUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		stream.pushGroup("material");

		if(stream.readString("type") != "material")
			throw Exception("Resource is not a material");

		Material* material = new Material(stream.readString("name"), 0);
		material->manager = &manager;
		std::string effectName = stream.readString("effectName");
		ResourceId effectId = manager.registerResource(effectName, Effect::TYPE);
		material->effectId = effectId;

		int texCount = stream.readInt("textureCount");
		for(int i = 0; i < texCount; i++) {
			stream.pushGroup("texture");

			std::string textureName = stream.readString("textureName");
			std::string resourceName = stream.readString("resourceName");

			stream.popGroup();

			ResourceId textureId = manager.registerResource(resourceName, Texture::TYPE);

			material->samplers.insert(std::make_pair(textureName, textureId));
		}

		return material;
	}

	void MaterialUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Material* material = dynamic_cast<Material*>((Material*)instance);

		if(!material)
			throw Exception("Resource is not a material");

		stream.pushGroup("material");

		Effect* effect = (Effect*)manager.getResource(material->effectId);

		stream.writeString("type", material->getType().getName());
		stream.writeString("name", material->name);
		stream.writeString("effectName", effect->getName());

		stream.writeInt("textureCount", material->samplers.size());

		std::map<std::string, ResourceId>::iterator texture = material->samplers.begin();
		while(texture != material->samplers.end()) {
			stream.pushGroup("texture");

			Texture* tex = (Texture*)manager.getResource(texture->second);

			stream.writeString("textureName", texture->first);
			stream.writeString("resourceName", tex->getName());

			stream.popGroup();
			texture++;
		}
	}

}  // namespace engine
