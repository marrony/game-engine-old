/*
 * TextureManager.cpp
 *
 *  Created on: 01/08/2012
 *      Author: marrony.neris
 */

#include "TextureManager.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Stream.h"
#include "System.h"

namespace engine {
#if 0

	int TextureManager::createTexture2D() {
		TextureManager::Tex tex;

		tex.type = TextureType::Texture2D;
		tex.height = 0;
		tex.width = 0;
		tex.format = ImageFormat::None;

		glGenTextures(1, &tex.texId);

		return textures.add(tex);
	}

	void TextureManager::destroyTexture(int handle) {
		if(handle == 0) return;

		TextureManager::Tex& tex = textures.get(handle);
		glDeleteTextures(1, &tex.texId);

		textures.remove(handle);
	}

	void TextureManager::setTextureData(int handle, int width, int height, int depth, ImageFormat format, const void* data) {
		if(handle == 0) return;

		TextureManager::Tex& tex = textures.get(handle);

		tex.width = width;
		tex.height = height;
		tex.depth = depth;
		tex.format = format;

		int textureUnit = 15;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &textureUnit);

		GLenum type;

		switch(tex.type) {
			case TextureType::Texture2D:
				type = GL_TEXTURE_2D;
				break;

			case TextureType::Texture3D:
				type = GL_TEXTURE_3D;
				break;

			case TextureType::TextureCube:
				type = GL_TEXTURE_CUBE_MAP;
				break;
		}

		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(type, tex.texId);

		int pixelType = data == 0 ? GL_FLOAT : GL_UNSIGNED_BYTE;

		int pixelFormat;
		int internalFormat;

		switch(depth) {
			case 3:
				pixelFormat = GL_RGB;
				break;

			case 4:
				pixelFormat = GL_RGBA;
				break;

			default:
				throw Exception("Unknow depth");
		}

		switch(format) {
			case ImageFormat::Rgb8:
				internalFormat = GL_RGB;
				break;

			case ImageFormat::Rgba8:
				internalFormat = GL_RGBA;
				break;

#ifndef ANDROID
			case ImageFormat::Rgba16Float:
				pixelType = GL_FLOAT;
				internalFormat = GL_RGBA16F;
				break;

			case ImageFormat::Rgba32Float:
				pixelType = GL_FLOAT;
				internalFormat = GL_RGBA32F;
				break;
#endif

			case ImageFormat::Depth:
				break;

			default:
				throw Exception("Unknow format");
		}

		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(type, 0, internalFormat, width, height, 0, pixelFormat, pixelType, data);

		glBindTexture(type, 0);
	}
#endif

} /* namespace engine */
