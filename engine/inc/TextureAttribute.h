/*
 * TextureAttribute.h
 *
 *  Created on: 13/03/2012
 *      Author: marrony.neris
 */

#ifndef TEXTUREATTRIBUTE_H_
#define TEXTUREATTRIBUTE_H_

#include <string>

#include "MaterialAttribute.h"

namespace engine {

	class Texture;

	class TextureAttribute : public MaterialAttribute {
	public:
		TextureAttribute(const std::string& name, Texture* texture);

		virtual void apply(Shader* effect);

	private:
		Texture* texture;
	};

}  // namespace engine

#endif /* TEXTUREATTRIBUTE_H_ */
