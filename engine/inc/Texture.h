/*
 * Texture.h
 *
 *  Created on: 22/10/2010
 *      Author: marrony.neris
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "DeviceObject.h"
#include "GraphicManagerExtra.h"

namespace engine {

	class Image;

	enum class TextureType {
		Texture2D, Texture3D, TextureCube
	};

	class Texture {
		Image* image;
		int handle;
		bool dirty;
	public:
		Texture();
		~Texture();

		int getHandle() const;
		void setHandle(int handle);

		void setImage(Image* image) {
			this->image = image;
			dirty = true;
		}
		Image* getImage() const { return image; }

		bool needUpdate() const;
		void setUpdated();

		TextureType getTextureType() const;
	};

}  // namespace engine

#endif /* TEXTURE_H_ */
