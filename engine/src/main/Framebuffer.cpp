/*
 * Framebuffer.cpp
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#include "Framebuffer.h"
#include "Texture.h"
#include "HardwareTexture.h"
#include "System.h"

#include <vector>

namespace engine {

	Framebuffer* const Framebuffer::DEFAULT = 0;

	void Framebuffer::setTexture(const Texture& texImpl, int target) {
		int textureId = 0;

		if(texImpl != 0) {
			textureId = ((HardwareTexture*)texImpl)->getTextureId();
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, target, GL_TEXTURE_2D, textureId, 0);
	}

	Framebuffer::Framebuffer(int _width, int _height) :
		width(_width), height(_height), colors(), depth(0), stencil(0) {
		glGenFramebuffers(1, &frameBufferId);
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers(1, &frameBufferId);
	}

	Texture* Framebuffer::getColorBuffer(int index) {
		return colors[index];
	}

	Texture* Framebuffer::getDepthBuffer() {
		return depth;
	}

	void Framebuffer::setRenderTarget(engine::Texture* texture, int index) {
		colors[index] = texture;
	}

	void Framebuffer::setDepthTarget(engine::Texture* texture) {
		depth = texture;
	}

	void Framebuffer::setStencilTarget(engine::Texture* texture) {
		stencil = texture;
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

		std::vector<GLenum> drawbuffers;
		for(ColorBuffer::iterator ite = colors.begin(); ite != colors.end(); ite++) {
			setTexture(ite->second, GL_COLOR_ATTACHMENT0 + ite->first);
			drawbuffers.push_back(GL_COLOR_ATTACHMENT0 + ite->first);
		}

		if(depth) {
			setTexture(depth, GL_DEPTH_ATTACHMENT);
		}

		if(stencil) {
			setTexture(stencil, GL_STENCIL_ATTACHMENT);
		}

#ifndef ANDROID
		glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);

		glDrawBuffers(drawbuffers.size(), drawbuffers.data());
#endif

		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, width, height);
	}

	void Framebuffer::unbind() {
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

#ifndef ANDROID
		glDrawBuffer(drawBuffer);
#endif

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}  // namespace engine
