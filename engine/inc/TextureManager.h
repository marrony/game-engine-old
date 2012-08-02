/*
 * TextureManager.h
 *
 *  Created on: 01/08/2012
 *      Author: marrony.neris
 */

#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <string>
#include <vector>

#include "ResourceReader.h"
#include "Resources.h"
#include "Texture.h"

namespace engine {

	class Texture;

	enum class TexType : unsigned short {
		Texture2D, Texture3D, TextureCube
	};

	class TextureManager {
	public:
		struct Tex {
			unsigned int texId;
			int width;
			int height;
			int depth;
			TexType type;
			TextureFormat format;
		};

		Resources<Tex> textures;

		int createTexture2D();
		void destroyTexture(int handle);
		void setTextureData(int handle, int width, int height, int depth, TextureFormat format, const void* data);
		void bindTexture(int handle, int unit);
	};

} /* namespace engine */

#endif /* TEXTUREMANAGER_H_ */
