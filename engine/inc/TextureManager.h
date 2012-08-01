/*
 * TextureManager.h
 *
 *  Created on: 01/08/2012
 *      Author: marrony.neris
 */

#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <string>

#include "ResourceLoader.h"

namespace engine {

	class ResourceManager;
	class Texture;

	class TextureManager : public ResourceLoader {
		ResourceManager& resourceManager;
	public:
		TextureManager(ResourceManager& resourceManager);

//		struct Tex {
//			unsigned int texId;
//			int width;
//			int height;
//			int depth;
//			TexType type;
//			TextureFormat format;
//		};
//
//		Resources<Tex> textures;
//
//		int createTexture2D();
//		void destroyTexture(int handle);
//		void setTextureData(int handle, int width, int height, int depth, TextureFormat format, const void* data);

		virtual Resource* loadResource(const std::string& name);
	};

} /* namespace engine */

#endif /* TEXTUREMANAGER_H_ */
