/*
 * Texture.h
 *
 *  Created on: 22/10/2010
 *      Author: marrony.neris
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"
#include "DeviceObject.h"
#include "GraphicManagerExtra.h"

namespace engine {

	class Image;

	enum class TextureFormat : unsigned char {
		None,
		Rgb8,
		Rgba8,
		Rgba16Float,
		Rgba32Float,
		Depth
	};

	enum class TextureType : unsigned short {
		Texture2D, Texture3D, TextureCube
	};

	class Texture : public Resource, public DeviceObject {
		int width;
		int height;
		int depth;
		bool uploaded;
		char* data;
		int handle;
		bool dirty;
		TextureFormat format;
		TextureType type;

		void cleanData();
	public:
		Texture(const std::string& name, ResourceManager* manager);

		virtual ~Texture();

		virtual Type getType() const;

		void markUploaded();

		virtual int getHandle() const;
		virtual void setHandle(int handle);

		const void* getData() const;

		int getWidth() const;

		int getHeight() const;

		int getDepth() const;

		void setData(int width, int height, int depth, const void* data);

		bool needUpdate() { return dirty; }
		void setUpdated() { dirty = false; }

		TextureType getTextureType() const { return TextureType::Texture2D; }
		TextureFormat getFormat() const { return depth == 4 ? TextureFormat::Rgba8 : TextureFormat::Rgb8; }

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
