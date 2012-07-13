/*
 * Framebuffer.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <map>

#include "Texture.h"

namespace engine {

	class Framebuffer {
		static const int MAX_COLORS = 8;

		int width;
		int height;
		unsigned int frameBufferId;
		int viewport[4];
		int drawBuffer;

		typedef std::map<int, Texture*> ColorBuffer;

		ColorBuffer colors;
		engine::Texture* depth;
		engine::Texture* stencil;

		void setTexture(const Texture& texImpl, int target);
	public:
		Framebuffer(int _width, int _height);
		~Framebuffer();

		Texture* getColorBuffer(int index);
		Texture* getDepthBuffer();

		void setRenderTarget(const Texture& texture, int index);
		void setDepthTarget(const Texture& texture);
		void setStencilTarget(const Texture& texture);

		void bind();
		void unbind();

		static Framebuffer* const DEFAULT;
	};

}  // namespace engine

#endif /* FRAMEBUFFER_H_ */
