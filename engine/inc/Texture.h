/*
 * Texture.h
 *
 *  Created on: 22/10/2010
 *      Author: marrony.neris
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"

namespace engine {

	class Image;

	enum TextureFormat {
		NONE,
		RGB8,
		RGBA8,
		RGBA16F,
		RGBA32F,
		DEPTH
	};

	class Texture : public Resource {
	public:
		int handle;
		class GraphicManager* manager;
	public:
		Texture(const std::string& name) :
				Resource(name), handle(0), manager(0) {
		}

		virtual Type getType() const {
			return Type("texture");
		}

		virtual void postLoaded() {
//			if(!manager) return;
//			if(handle) return;
//
//			handle = manager->createTexture2D();
		}
	};

	struct TextureUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	struct TextureEvent : public ResourceEvent {
		Texture* texture;
		Image* image;
	};

	class TextureKey : public ResourceKey {
	public:
		TextureKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "texture/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

}  // namespace engine

#endif /* TEXTURE_H_ */
