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
		Texture(const std::string& name);
		virtual ~Texture() { }

		virtual int getWidth() = 0;

		virtual int getHeight() = 0;

		virtual int getDepth() = 0;

		virtual void bind(int textureUnit) = 0;

		virtual void unbind() = 0;

		virtual void generateMipmap() = 0;

		virtual void setData(int level, Image* image) = 0;

		virtual void setData(int level, int width, int height, int depth, const void* data) = 0;

		virtual void getData(int level, void* data) = 0;

		virtual Type getType() const {
			return TYPE;
		}

		static const Type TYPE;
	private:
		friend class TextureUtils;
	};

	struct TextureUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

}  // namespace engine

#endif /* TEXTURE_H_ */
