/*
 * HardwareTexture.h
 *
 *  Created on: 02/04/2012
 *      Author: marrony.neris
 */

#ifndef HARDWARETEXTURE_H_
#define HARDWARETEXTURE_H_

#include "Texture.h"

namespace engine {

	class HardwareTexture : public Texture {
	public:
		HardwareTexture(const std::string& name, TextureFormat format);
		virtual ~HardwareTexture();

		virtual int getWidth();

		virtual int getHeight();

		virtual int getDepth();

		virtual unsigned int getTextureId();

		virtual void bind(int textureUnit);

		virtual void unbind();

		virtual void generateMipmap();

		virtual void setData(int level, Image* image);

		virtual void setData(int level, int width, int height, int depth, const void* data);

		virtual void getData(int level, void* data);
	private:
		TextureFormat format;
		unsigned int textureId;
		int width;
		int height;
		int unit;
	};

} /* namespace engine */

#endif /* HARDWARETEXTURE_H_ */
