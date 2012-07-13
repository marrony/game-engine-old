/*
 * MemoryTexture.cpp
 *
 *  Created on: 02/04/2012
 *      Author: marrony.neris
 */

#include "MemoryTexture.h"
#include "Image.h"
#include "string.h"

namespace compiler {

	MemoryTexture::MemoryTexture(const std::string& name) :
			Texture(name) {
	}

	MemoryTexture::~MemoryTexture() {
		for(size_t i = 0; i < images.size(); i++)
			delete images[i];
	}

	int MemoryTexture::getWidth() {
		return images[0]->getWidth();
	}

	int MemoryTexture::getHeight() {
		return images[0]->getHeight();
	}

	int MemoryTexture::getDepth() {
		return images[0]->getDepth();
	}

	unsigned int MemoryTexture::getTextureId() {
		return 0;
	}

	void MemoryTexture::bind(int textureUnit) {
	}

	void MemoryTexture::unbind() {
	}

	void MemoryTexture::generateMipmap() {
	}

	void MemoryTexture::setData(int level, engine::Image* image) {
		setData(level, image->getWidth(), image->getHeight(), image->getDepth(), image->getData());
	}

	void MemoryTexture::setData(int level, int width, int height, int depth, const void* data) {
		engine::Image* image = new engine::Image(width, height, depth, data);

		images[level] = image;
	}

	void MemoryTexture::getData(int level, void* data) {
		int size = images[level]->getWidth() * images[level]->getHeight() * images[level]->getDepth();

		memcpy(data, images[level]->getData(), size);
	}
}

/* namespace compiler */
