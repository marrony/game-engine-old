/*
 * Framebuffer.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <map>

namespace engine {

	class Texture;

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

		void setTexture(engine::Texture* texImpl, int target);
	public:
		Framebuffer(int _width, int _height);
		~Framebuffer();

		Texture* getColorBuffer(int index);
		Texture* getDepthBuffer();

		void setRenderTarget(Texture* texture, int index);
		void setDepthTarget(Texture* texture);
		void setStencilTarget(Texture* texture);

		void bind();
		void unbind();

		static Framebuffer* const DEFAULT;
	};

}  // namespace engine

#endif /* FRAMEBUFFER_H_ */
