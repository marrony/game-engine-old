/*
 * Texture.h
 *
 *  Created on: 22/10/2010
 *      Author: marrony.neris
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"
#include "GraphicManagerExtra.h"

namespace engine {

	class Image;
	class TextureManager;

	enum class TextureFormat : unsigned char {
		None,
		Rgb8,
		Rgba8,
		Rgba16Float,
		Rgba32Float,
		Depth
	};

	class Texture : public Resource {
		int width;
		int height;
		int depth;
		bool uploaded;
		char* data;
		int handle;

		TextureManager* textureManager;

		void cleanData();
	public:
		Texture(const std::string& name, ResourceManager* manager, TextureManager* textureManager);

		virtual ~Texture();

		virtual Type getType() const;

		void markUploaded();

		void setHandle(int handle);

		int getHandle() const;

		const void* getData() const;

		int getWidth() const;

		int getHeight() const;

		int getDepth() const;

		void setData(int width, int height, int depth, const void* data);

		void bind(int unit);

		friend class TextureUtils;
	};

	struct TextureUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
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
