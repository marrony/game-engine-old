/*
 * TextureAttribute.cpp
 *
 *  Created on: 13/03/2012
 *      Author: marrony.neris
 */

#include "TextureAttribute.h"
#include "Texture.h"
#include "Shader.h"

namespace engine {

	TextureAttribute::TextureAttribute(const std::string& name, Texture* texture) :
			MaterialAttribute(name), texture(texture) {
	}

	void TextureAttribute::apply(Shader* effect) {
		//effect->bindTextureUnit(name, texture);
	}

} // namespace engine
