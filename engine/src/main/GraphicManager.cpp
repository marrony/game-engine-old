/*
 * GraphicManager.cpp
 *
 *  Created on: 06/03/2011
 *      Author: marrony
 */

#include "GraphicManager.h"
#include "Texture.h"
#include "Image.h"
#include "Model.h"
#include "Geometry.h"
#include "Frustum.h"
#include "Light.h"

namespace engine {

	GraphicManager::GraphicManager() :
			width(0), height(0), framebuffer(0), shader(0) {
		usedAttributes = 0;
		usedTextures = 0;
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
//			if(!framebuffer) {
//				if(lastFramebuffer)
//					lastFramebuffer->unbind();
//			} else
//				framebuffer->bind();
//
//			lastFramebuffer = framebuffer;
		}

		if(flags & ShaderAltered)
			shader->bind();

		if(flags & VertexBufferAltered)
			vertexBuffer->bind();

		if(flags & IndexBufferAltered)
			indexBuffer->bind();

		if(flags & TextureAltered) {
			for(int i = 0; i < 16; ++i) {
				int mask = (1 << i);

				if(usedTextures & mask) {
					shader->getConstant(textureName[i])->setValue(i);

					Tex& tex = textures0.get(textures[i]->handle);

					glActiveTexture(GL_TEXTURE0 + i);
					#ifndef ANDROID
					//glClientActiveTexture(GL_TEXTURE0 + i);
					#endif
					glBindTexture(GL_TEXTURE_2D, tex.texId);
				}
			}

			for(int i = 0; i < 16; ++i) {
				int mask = (1 << i);

				if(!(usedTextures & mask) && (lastUsedTextures & mask)) {
					glActiveTexture(GL_TEXTURE0 + i);
					#ifndef ANDROID
					//glClientActiveTexture(GL_TEXTURE0 + i);
					#endif
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}

			lastUsedTextures = usedTextures;
			usedTextures = 0;
		}

		if(flags & AttributesAltered) {
			for(int i = 0; i < AttributeOffset::MaxAttributeOffset; ++i) {
				int mask = (1 << i);

				if(usedAttributes & mask) {
					Attribs attr = attribs[i];

					glEnableVertexAttribArray(attr.attrib);
					glVertexAttribPointer(attr.attrib, attr.mode, GL_FLOAT, GL_FALSE, attr.stride, (void*)attr.offset);
				}
			}

			for(int i = 0; i < AttributeOffset::MaxAttributeOffset; ++i) {
				int mask = (1 << i);

				if(!(usedAttributes & mask) && (lastUsedAttributes & mask)) {
					Attribs attr = attribs[i];

					glDisableVertexAttribArray(attr.attrib);
				}
			}

			lastUsedAttributes = usedAttributes;
			usedAttributes = 0;
		}

		if(flags & ConstantsAltered) {
			const math::Matrix4 modelViewMatrix = context.frustum->getViewMatrix() * context.geometry->getTransformation();

			for(ConstantsEnabled c : constants) {
				Constant* cc = shader->getConstant(c.name);

				switch(c.constant) {
				case Constants::ZNear:
					cc->setValue(context.frustum->getNear());
					break;

				case Constants::ZFar:
					cc->setValue(context.frustum->getFar());
					break;

				case Constants::ProjectionMatrix:
					cc->setValue(context.frustum->getProjectionMatrix());
					break;

				case Constants::ModelViewMatrix:
					cc->setValue(modelViewMatrix);
					break;

				case Constants::NormalMatrix:
					cc->setValue(modelViewMatrix.normalMatrix());
					break;

				case Constants::LightPosition:
					if(context.light) {
						cc->setValue(context.frustum->getViewMatrix() * context.light->getPosition());
					} else {
						cc->setValue(context.frustum->getViewMatrix() * math::Vector3(0, 20, 0));
					}
					break;

				case Constants::LightColor:
					if(context.light) {
						cc->setValue(context.light->getDiffuse());
					} else {
						cc->setValue(math::Vector3(1, 1, 1));
					}
					break;

				case Constants::ObjectId:
					cc->setValue(context.objectId);
					break;

				case Constants::BonePallete:
					size_t bonesCount = context.model->getAnimation().getBonesCount();

					if(!context.model->geometry->boneIds.empty() && !context.model->geometry->weights.empty() && bonesCount > 0) {
						cc->setValue(context.geometry->getBoneMatrix(), bonesCount);
					} else {
						cc->setValue(&math::Matrix4::IDENTITY, 1);
					}

					break;
				}
			}
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

	void GraphicManager::drawIndex(int start, int end, int count, int offset) {
		commitModifications();

		unsigned short* ptr = (unsigned short*) 0;

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
			int mask = (1 << i);

			if(!(usedTextures & mask)) {
				usedTextures |= mask;
				textures[i] = texture;
				textureName[i] = name;
				break;
			}
		}

		flags |= TextureAltered;
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

	void GraphicManager::setAttribute(AttributeOffset attributeOffset, int index, int mode, int offset, int stride) {
		attribs[attributeOffset].attrib = index;
		attribs[attributeOffset].mode = mode;
		attribs[attributeOffset].offset = offset;
		attribs[attributeOffset].stride = stride;

		usedAttributes |= (1 << attributeOffset);

		flags |= AttributesAltered;
	}

	void GraphicManager::onTexture(Texture* texture, Image* image) {
		texture->handle = createTexture2D();

		TextureFormat format = image->getDepth() == 3 ? TextureFormat::RGB8 : TextureFormat::RGBA8;

		setTextureData(texture->handle, image->getWidth(), image->getHeight(), image->getDepth(), format, image->getData());
	}

	void GraphicManager::onMaterial(Material* material) {
	}

	void GraphicManager::onEffect(Effect* effect) {
		effect->finalizeInitialization();
	}

	void GraphicManager::onShader(Shader* shader) {
	}

	void GraphicManager::onModel(Model* model) {
		model->uploadData(this);
	}

	int GraphicManager::createTexture2D() {
		Tex tex;

		tex.type = Texture2D;
		tex.height = 0;
		tex.width = 0;
		tex.format = TextureFormat::NONE;

		glGenTextures(1, &tex.texId);

		return (int)textures0.add(tex);
	}

	void GraphicManager::destroyTexture(int handle) {
		if(handle == 0) return;

		Tex& tex = textures0.get(handle);
		glDeleteTextures(1, &tex.texId);

		textures0.remove(handle);
	}

	void GraphicManager::setTextureData(int handle, int width, int height, int depth, TextureFormat format, const void* data) {
		if(handle == 0) return;

		Tex& tex = textures0.get(handle);

		tex.width = width;
		tex.height = height;
		tex.depth = depth;
		tex.format = format;

		int textureUnit = 15;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &textureUnit);

		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(tex.type, tex.texId);

		int pixelType = data == 0 ? GL_FLOAT : GL_UNSIGNED_BYTE;

		int pixelFormat;
		int internalFormat;

		switch(depth) {
			case 3:
				pixelFormat = GL_RGB;
				break;

			case 4:
				pixelFormat = GL_RGBA;
				break;

			default:
				throw Exception("Unknow depth");
		}

		switch(format) {
			case TextureFormat::RGB8:
				internalFormat = GL_RGB;
				break;

			case TextureFormat::RGBA8:
				internalFormat = GL_RGBA;
				break;

#ifndef ANDROID
			case TextureFormat::RGBA16F:
				pixelType = GL_FLOAT;
				internalFormat = GL_RGBA16F;
				break;

			case TextureFormat::RGBA32F:
				pixelType = GL_FLOAT;
				internalFormat = GL_RGBA32F;
				break;
#endif

			case TextureFormat::DEPTH:
				break;

			default:
				throw Exception("Unknow format");
		}

		glTexParameteri(tex.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(tex.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(tex.type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(tex.type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(tex.type, 0, internalFormat, width, height, 0, pixelFormat, pixelType, data);

		glBindTexture(tex.type, 0);
	}

	Buffer* GraphicManager::createBuffer(int count, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess) {
		return new HardwareBuffer(count, bufferType, frequencyAccess, natureAccess);
	}
}  // namespace engine
