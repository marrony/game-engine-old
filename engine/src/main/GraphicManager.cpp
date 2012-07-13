/*
 * GraphicManager.cpp
 *
 *  Created on: 06/03/2011
 *      Author: marrony
 */

#include "GraphicManager.h"
#include "Texture.h"

namespace engine {

	GraphicManager::GraphicManager() :
			width(0), height(0), framebuffer(0), shader(0) {
		usedAttributes = 0;
		lastUsedAttributes = 0;
		lastUsedTextures = 0;
		flags = 0;
		vertexBuffer = 0;
		indexBuffer = 0;

		memset(textures, 0, sizeof(textures));
	}

	GraphicManager::~GraphicManager() {
	}

	void GraphicManager::commitModifications() {
		if(flags == 0) return;

		if(flags & FramebufferAltered) {
			if(!framebuffer) {
				if(lastFramebuffer)
					lastFramebuffer->unbind();
			} else
				framebuffer->bind();

			lastFramebuffer = framebuffer;
		}

		if(flags & ShaderAltered)
			shader->bind();

		if(flags & VertexBufferAltered)
			vertexBuffer->bind();

		if(flags & IndexBufferAltered)
			indexBuffer->bind();

		if(flags & TextureAltered) {
			int usedTextures = 0;

			for(int i = 0; i < 16; ++i) {
				int mask = (1 << i);

				if(textures[i]) {
					usedTextures |= mask;

					shader->getConstant(textureName[i])->setValue(i);
					textures[i]->bind(i);
				}
			}

			for(int i = 0; i < 16; ++i) {
				int mask = (1 << i);

				if(!(usedTextures & mask) && (lastUsedTextures & mask)) {
					textures[i]->unbind();
				}
			}

			for(int i = 0; i < 16; ++i)
				textures[i] = 0;

			lastUsedTextures = usedTextures;
		}

		if(flags & AttributesAltered) {
			for(int i = 0; i < AttributeOffset::MaxAttributeOffset; ++i) {
				int mask = (1 << i);

				if(usedAttributes & mask) {
					Attribs& attr = attribs[i];

					attr.attribute->setValue(attr.mode, attr.offset, attr.stride);
					attr.attribute->enable();
				}
			}

			for(int i = 0; i < AttributeOffset::MaxAttributeOffset; ++i) {
				int mask = (1 << i);

				if(!(usedAttributes & mask) && (lastUsedAttributes & mask)) {
					Attribs& attr = attribs[i];
					attr.attribute->disable();
				}
			}

			lastUsedAttributes = usedAttributes;
			usedAttributes = 0;
		}

		flags = 0;
	}

	void GraphicManager::resize(int newWidth, int newHeight) {
		width = newWidth;
		height = newHeight;

		glViewport(0, 0, width, height);
	}

	int GraphicManager::getWidth() {
		return width;
	}

	int GraphicManager::getHeight() {
		return height;
	}

	void GraphicManager::cullFace(int face, int order) {
		glEnable(GL_CULL_FACE);
		glCullFace(face);
		glFrontFace(order);
	}

	void GraphicManager::disableCullFace() {
		glDisable(GL_CULL_FACE);
	}

	void GraphicManager::enableAditiveBlend() {
		blend(GL_ONE, GL_ONE);
	}

	void GraphicManager::blend(Blend::Equation blendEquation) {
		glEnable(GL_BLEND);

		switch(blendEquation) {
		case Blend::ADD:
			glBlendEquation(GL_FUNC_ADD);
			break;

		case Blend::SUBTRACT:
			glBlendEquation(GL_FUNC_SUBTRACT);
			break;

		case Blend::REVERSE_SUBTRACT:
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			break;

		case Blend::MIN:
#ifndef ANDROID
			glBlendEquation(GL_MIN);
#endif
			break;

		case Blend::MAX:
#ifndef ANDROID
			glBlendEquation(GL_MAX);
#endif
			break;

		default:
			break;
		}
	}

	void GraphicManager::blend(int sfactor, int dfactor) {
		glEnable(GL_BLEND);
		glBlendFunc(sfactor, dfactor);
	}

	void GraphicManager::disableBlend() {
		glDisable(GL_BLEND);
	}

	void GraphicManager::scissorTest(const engine::math::Vector4& scissor) {
		glEnable(GL_SCISSOR_TEST);
		glScissor((int)scissor.x, (int)scissor.y, (int)scissor.z, (int)scissor.w);
	}

	void GraphicManager::disableScissor() {
		glDisable(GL_SCISSOR_TEST);
	}

	void GraphicManager::depthTest(int depthFunc) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(depthFunc);
		enableDepthWrite();
	}

	void GraphicManager::disableDepthTest() {
		glDisable(GL_DEPTH_TEST);
	}

	void GraphicManager::clear(const engine::math::Vector4& color, float depth) {
		enableColorWrite();
		enableDepthWrite();

		glClearColor(color.x, color.y, color.z, color.w);

#ifndef ANDROID
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glClearDepth(depth);
#endif

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GraphicManager::clear(const engine::math::Vector4& color) {
		enableColorWrite();

		glClearColor(color.x, color.y, color.z, color.w);

#ifndef ANDROID
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
#endif

		glClear(GL_COLOR_BUFFER_BIT);
	}

	void GraphicManager::bindFramebuffer(engine::Framebuffer* const newFramebuffer) {
		if(framebuffer == newFramebuffer) return;

		framebuffer = newFramebuffer;
		flags |= FramebufferAltered;
	}

	void GraphicManager::setShader(engine::Shader* const newShader) {
		if(shader == newShader) return;

		shader = newShader;
		flags |= ShaderAltered;
	}

	void GraphicManager::unbindShader() {
		if(shader != 0) {
			shader->unbind();
			shader = 0;

			for(int i = 0; i < 16; i++) {
				if(textures[i])
					textures[i]->unbind();
			}

			memset(textures, 0, sizeof(textures));
		}
	}

	void GraphicManager::enableDepthWrite() {
		glDepthMask(GL_TRUE);
	}

	void GraphicManager::disableDepthWrite() {
		glDepthMask(GL_FALSE);
	}

	void GraphicManager::enableColorWrite() {
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	void GraphicManager::disableColorWrite() {
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}

	void GraphicManager::enableTexture() {
		glEnable(GL_TEXTURE_2D);
	}

	void GraphicManager::disableTexture() {
		glDisable(GL_TEXTURE_2D);
	}

	void GraphicManager::drawIndex(Buffer* indexBuffer, int start, int end, int count, int offset) {
		commitModifications();

		unsigned short* ptr = (unsigned short*) indexBuffer->getPointer();
#ifndef ANDROID
		glDrawRangeElements(GL_TRIANGLES, start, end, count, GL_UNSIGNED_SHORT, ptr + offset);
#endif
	}

	void GraphicManager::drawIndex(const unsigned short* index, unsigned int count) {
		commitModifications();

		//glDrawRangeElements
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, index);
	}

	void GraphicManager::bindTexture(const std::string& name, Texture* texture) {
		for(int i = 0; i < 16; i++) {
			if(textures[i] == 0) {
				textures[i] = texture;
				textureName[i] = name;
				break;
			}
		}

		flags |= TextureAltered;
	}

	void GraphicManager::addSampler(const std::string& name, Texture* sampler) {
		samplers[name] = sampler;
	}

	void GraphicManager::removeSamplers() {
		samplers.clear();
	}

	void GraphicManager::setVertexBuffer(Buffer* vertexBuffer) {
		if(this->vertexBuffer == vertexBuffer) return;

		this->vertexBuffer = vertexBuffer;
		flags |= VertexBufferAltered;
	}

	void GraphicManager::setIndexBuffer(Buffer* indexBuffer) {
		if(this->indexBuffer == indexBuffer) return;

		this->indexBuffer = indexBuffer;
		flags |= IndexBufferAltered;
	}

	void GraphicManager::setAttribute(int index, Attribute* attr, int mode, int offset, int stride) {
		Attribs& attrib = attribs[index];

		attrib.attribute = attr;
		attrib.mode = mode;
		attrib.offset = offset;
		attrib.stride = stride;

		usedAttributes |= (1 << index);

		flags |= AttributesAltered;
	}

}  // namespace engine
