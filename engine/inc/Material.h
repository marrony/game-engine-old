/*
 * Material.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Resource.h"

#include <vector>
#include <string>
#include <map>

namespace engine {

	class Effect;

	class Material : public Resource {
	public:
		Material(const std::string& name, ResourceManager* manager, Effect* effect);
		virtual ~Material();

		void addSampler(const std::string& samplerName, class Texture* sampler);
		class Texture* getSampler(const std::string& samplerName);

		class Effect* getEffect() {
			return effect;
		}

		void begin(const std::string& aspect, class ConstantContext& context);
		void end();

		virtual Type getType() const {
			return TYPE;
		}

		static const Type TYPE;
	private:
		class Effect* effect;
		std::map<std::string, class Texture*> samplers;

		friend class MaterialUtils;
	};

	struct MaterialUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	class MaterialKey : public ResourceKey {
	public:
		MaterialKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "material/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

}  // namespace engine

#endif /* MATERIAL_H_ */
