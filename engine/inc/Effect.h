/*
 * Effect.h
 *
 *  Created on: 20/07/2012
 *      Author: marrony.neris
 */

#ifndef EFFECT_H_
#define EFFECT_H_

#include "Resource.h"

namespace engine {

	enum Constants {
		ZNear,
		ZFar,
		ProjectionMatrix,
		ModelViewMatrix,
		NormalMatrix,
		LightPosition,
		LightColor,
		ObjectId,
		BonePallete,
	};

	enum AttributeOffset {
		PositionOffset = 0,
		NormalOffset,
		STangentOffset,
		TTangentOffset,
		ColorOffset,
		TexCoordOffset,
		BoneIdsOffset,
		WeigthsOffset,
		MaxAttributeOffset
	};

	struct ConstantContext {
		class GraphicManager* graphicManager;
		class ResourceManager* resourceManager;
		class ModelInstance* geometry;
		class Frustum* frustum;
		class Light* light;
		class Model* model;
		float objectId;
	};

	struct AttributeEnabled {
		std::string name;
		bool enabled;
		int mode;
		int index;

		AttributeEnabled() :
				name(), enabled(false), mode(0) {
		}
	};

	struct ConstantsEnabled {
		std::string name;
		Constants constant;

		ConstantsEnabled(const std::string& name, Constants constant) :
				name(name), constant(constant) {
		}
	};

	class Effect : public Resource {
	public:
		Effect(const std::string& name, ResourceManager* manager) : Resource(name, manager) {
			usingDephtWrite = false;
			usingColorWrite = false;
			usingDepthTest = false;
			usingBlend = false;
			usingCull = false;
		}
		virtual ~Effect();

		virtual Type getType() const {
			return TYPE;
		}

		void enableDepthWrite(bool depthWriteEnabled);
		void enableColorWrite(bool colorWriteEnabled);
		void depthTest(int depthFunc);
		void blendMode(int blendMode);
		void cullMode(int cullMode);

		void begin(ConstantContext& context);
		void end(ConstantContext& context);

		void finalizeInitialization();

		void setAttribute(const std::string& name, const std::string& semmantic);

		void setupAttributes(class Model* model, class Shader* shader, class GraphicManager* graphicManager);

		void setShader(Shader* s) {
			shader = s;
		}

		Shader* getShader() {
			return shader;
		}

		static const Type TYPE;
	private:
		Shader* shader;

		bool usingDephtWrite : 1;
		bool usingColorWrite : 1;
		bool usingDepthTest : 1;
		bool usingBlend : 1;
		bool usingCull : 1;

		bool depthWriteEnabled;
		bool colorWriteEnabled;
		int depthFunc;
		int _blendMode;
		int _cullMode;

		AttributeEnabled attributesEnabled[AttributeOffset::MaxAttributeOffset];
		std::vector<ConstantsEnabled> constants;

		friend class EffectUtils;
	};

	struct EffectUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	class EffectKey : public ResourceKey {
	public:
		EffectKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "effect/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

} /* namespace engine */

#endif /* EFFECT_H_ */
