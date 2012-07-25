/*
 * Effect.cpp
 *
 *  Created on: 20/07/2012
 *      Author: marrony.neris
 */

#include "Effect.h"

#include "ResourceManager.h"
#include "GraphicManager.h"
#include "Model.h"

namespace engine {

	const Type Effect::TYPE("effect");

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

		context.graphicManager->setConstant(context, constants);
		setupAttributes(context.model, shader, context.graphicManager);
	}

	void Effect::finalizeInitialization() {
		shader->link();

		for(int i = 0; i < AttributeOffset::MaxAttributeOffset; ++i) {
			AttributeEnabled& attrib = attributesEnabled[i];

			if(attrib.enabled)
				attrib.index = shader->getAttributeIndex(attrib.name);
		}
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

	void Effect::setupAttributes(Model* model, Shader* shader, GraphicManager* graphicManager) {
		ModelData* modelData = model->modelData;

		for(int offset = 0; offset < AttributeOffset::MaxAttributeOffset; ++offset) {
			AttributeEnabled& att = attributesEnabled[offset];

			if(!att.enabled) continue;

			graphicManager->setAttribute((AttributeOffset)offset, att.index, att.mode, modelData->attributeOffsets[offset], sizeof(float) * modelData->elementsPerVertex);
		}
	}

	Effect::~Effect() {
		delete shader;
	}

	void* EffectUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		stream.pushGroup("effect");

		std::string type = stream.readString("type");
		std::string name = stream.readString("name");

		std::string vs = stream.readString("vertexShader");
		std::string fs = stream.readString("fragmentShader");

		Source* vss = new Source(vs);
		Source* fss = new Source(fs);

		Effect* effect = new Effect(name, &manager);

		effect->shader = new Shader(vss, fss, 0);
		//effect->shader->link();

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

		effect->constants.push_back(ConstantsEnabled("myZNear", Constants::ZNear));
		effect->constants.push_back(ConstantsEnabled("myZFar", Constants::ZFar));
		effect->constants.push_back(ConstantsEnabled("myProjectionMatrix", Constants::ProjectionMatrix));
		effect->constants.push_back(ConstantsEnabled("myModelViewMatrix", Constants::ModelViewMatrix));
		effect->constants.push_back(ConstantsEnabled("myNormalMatrix", Constants::NormalMatrix));
		effect->constants.push_back(ConstantsEnabled("myLightPosition", Constants::LightPosition));
		effect->constants.push_back(ConstantsEnabled("myLightColor", Constants::LightColor));
		effect->constants.push_back(ConstantsEnabled("myObjectId", Constants::ObjectId));
		effect->constants.push_back(ConstantsEnabled("myBonePalete", Constants::BonePallete));

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

	Resource* EffectKey::loadResource(class ResourceManager& manager) const {
		std::string effectName = getName();

		FileStream fileStream("resources/materials/" + effectName + ".effect");
		ResourceBinStream resourceStream(fileStream);
		Effect* effect = (Effect*)EffectUtils::read(resourceStream, manager, 0);

		EffectEvent event;
		event.type = "effect";
		event.effect = effect;

		manager.dispatchLoadedEvent(event);

		return effect;
	}

} /* namespace engine */
