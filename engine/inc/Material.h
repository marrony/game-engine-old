/*
 * Material.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Resource.h"
#include "Texture.h"
#include "math/Vector.h"

#include <vector>
#include <string>
#include <map>

namespace engine {

	class MaterialAttribute;
	class Texture;
	class Shader;
	class Constant;
	class GraphicManager;
	class SceneManager;
	class ResourceManager;

	/**
	 * Essa classe pode ser compartilhada por varios Material
	 */
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
		class Geometry* geometry;
		class Frustum* frustum;
		class Light* light;
		class Model* model;
		float objectId;
	};

	class Effect : public Resource {
	public:
		Effect(const std::string& name) : Resource(name) {
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

		void setAttribute(const std::string& name, const std::string& semmantic);

		void setupConstants(ConstantContext& context);
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

		struct AttributeEnabled {
			std::string name;
			bool enabled;
			int mode;

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

		AttributeEnabled attributesEnabled[AttributeOffset::MaxAttributeOffset];
		std::vector<ConstantsEnabled> constants;

		friend class EffectUtils;
	};

	struct EffectUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	class Material : public Resource {
	public:
		Material(const std::string& name, ResourceId effectId);
		virtual ~Material();

		void addSampler(const std::string& samplerName, ResourceId sampler);
		ResourceId getSampler(const std::string& samplerName);

		ResourceId getEffect() {
			return effectId;
		}

		void begin(const std::string& aspect, ConstantContext& context);
		void end();

		virtual Type getType() const {
			return TYPE;
		}

		static const Type TYPE;
	private:
		ResourceId effectId;
		std::map<std::string, ResourceId> samplers;

		friend class MaterialUtils;
	};

	struct MaterialUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

}  // namespace engine

#endif /* MATERIAL_H_ */
