/*
 * Texture.cpp
 *
 *  Created on: 04/07/2011
 *      Author: marrony
 */

#include "Texture.h"
#include "Image.h"
#include "ResourceManager.h"

namespace engine {

	Texture::Texture() :
			image(0), handle(0), dirty(true) {
	}

	Texture::~Texture() {
	}

	void Texture::setHandle(int handle) {
		this->handle = handle;
	}

	int Texture::getHandle() const {
		return handle;
	}

	bool Texture::needUpdate() const {
		return dirty;
	}

	void Texture::setUpdated() {
		dirty = false;
	}

	TextureType Texture::getTextureType() const {
		return TextureType::Texture2D;
	}

} // namespace engine
