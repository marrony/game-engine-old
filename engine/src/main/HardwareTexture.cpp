/*
 * HardwareTexture.cpp
 *
 *  Created on: 02/04/2012
 *      Author: marrony.neris
 */

#include "HardwareTexture.h"
#include "Image.h"
#include "System.h"
#include "Exception.h"

#include "MemoryManager.h"

namespace engine {

	HardwareTexture::HardwareTexture(const std::string& name, TextureFormat format) :
			Texture(name), format(format), textureId(0), unit(0) {
	}

	HardwareTexture::~HardwareTexture() {
		glDeleteTextures(1, &textureId);
	}

	unsigned int HardwareTexture::getTextureId() {
		return textureId;
	}

	int HardwareTexture::getWidth() {
		return width;
	}

	int HardwareTexture::getHeight() {
		return height;
	}

	int HardwareTexture::getDepth() {
		switch(format) {
		case TextureFormat::RGB8:
			return 3;

		case TextureFormat::RGBA8:
			return 4;

		case TextureFormat::RGBA16F:
			return 4;

		case TextureFormat::RGBA32F:
			return 4;
		}

		return 0;
	}

	void HardwareTexture::bind(int textureUnit) {
		unit = textureUnit;
		glActiveTexture(GL_TEXTURE0 + unit);
#ifndef ANDROID
		//glClientActiveTexture(GL_TEXTURE0 + unit);
#endif
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void HardwareTexture::unbind() {
		glActiveTexture(GL_TEXTURE0 + unit);
#ifndef ANDROID
		//glClientActiveTexture(GL_TEXTURE0 + unit);
#endif
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void HardwareTexture::generateMipmap() {
#ifndef ANDROID
		glBindTexture(GL_TEXTURE_2D, textureId);
		glEnable(GL_TEXTURE_2D);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
	}

	void HardwareTexture::setData(int level, Image* image) {
		setData(level, image->getWidth(), image->getHeight(), image->getDepth(), image->getData());
	}

	void HardwareTexture::setData(int level, int width, int height, int depth, const void* data) {
		if(textureId == 0) {
			glGenTextures(1, &textureId);
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
		glEnable(GL_TEXTURE_2D);

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
			case TextureFormat::RGB8:
				internalFormat = GL_RGB;
				break;

			case TextureFormat::RGBA8:
				internalFormat = GL_RGBA;
				break;

#ifndef ANDROID
			case TextureFormat::RGBA16F:
				pixelType = GL_FLOAT;
				internalFormat = GL_RGBA16F;
				break;

			case TextureFormat::RGBA32F:
				pixelType = GL_FLOAT;
				internalFormat = GL_RGBA32F;
				break;
#endif

			case TextureFormat::DEPTH:
				break;

			default:
				throw Exception("Unknow format");
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, pixelFormat, pixelType, data);

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void HardwareTexture::getData(int level, void* data) {
		int pixelFormat;

		switch(format) {
		case TextureFormat::RGB8:
			pixelFormat = GL_RGB;
			break;

		case TextureFormat::RGBA8:
			pixelFormat = GL_RGBA;
			break;

		case TextureFormat::RGBA16F:
			pixelFormat = GL_RGBA;
			break;

		case TextureFormat::RGBA32F:
			pixelFormat = GL_RGBA;
			break;

		case TextureFormat::DEPTH:
			break;

		default:
			throw Exception("Unknow format");
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
#ifndef ANDROID
		glGetTexImage(GL_TEXTURE_2D, level, pixelFormat, GL_UNSIGNED_BYTE, data);
#endif
		glBindTexture(GL_TEXTURE_2D, 0);
	}

} /* namespace engine */
