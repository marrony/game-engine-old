/*
 * MemoryTexture.h
 *
 *  Created on: 02/04/2012
 *      Author: marrony.neris
 */

#ifndef MEMORYTEXTURE_H_
#define MEMORYTEXTURE_H_

#include "Texture.h"

#include <map>

namespace engine {
	class Image;
}

namespace compiler {

	class MemoryTexture : public engine::Texture {
	public:
		MemoryTexture(const std::string& name);
		virtual ~MemoryTexture();

		virtual int getWidth();

		virtual int getHeight();

		virtual int getDepth();

		virtual unsigned int getTextureId();

		virtual void bind(int textureUnit);

		virtual void unbind();

		virtual void generateMipmap();

		virtual void setData(int level, engine::Image* image);

		virtual void setData(int level, int width, int height, int depth, const void* data);

		virtual void getData(int level, void* data);
	private:
		std::map<int, engine::Image*> images;
	};

} /* namespace compiler */

#endif /* MEMORYTEXTURE_H_ */
