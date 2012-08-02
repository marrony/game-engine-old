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
		usedTexturesSlots = 0;
		lastUsedAttributes = 0;
		lastUsedTexturesSlots = 0;
		flags = 0;
		vertexBuffer = 0;
		indexBuffer = 0;

		memset(texturesUsed, 0, sizeof(texturesUsed));
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
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		if(flags & IndexBufferAltered)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		if(flags & TextureAltered) {
			for(int i = 0; i < 16; ++i) {
				int mask = (1 << i);

				if(usedTexturesSlots & mask) {
					shader->getConstant(textureName[i])->setValue(i);

					texturesUsed[i]->bind(i);
				}
			}

			for(int i = 0; i < 16; ++i) {
				int mask = (1 << i);

				if(!(usedTexturesSlots & mask) && (lastUsedTexturesSlots & mask)) {
					glActiveTexture(GL_TEXTURE0 + i);
					#ifndef ANDROID
					//glClientActiveTexture(GL_TEXTURE0 + i);
					#endif
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}

			lastUsedTexturesSlots = usedTexturesSlots;
			usedTexturesSlots = 0;
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

					if(!context.model->modelData->boneIds.empty() && !context.model->modelData->weights.empty() && bonesCount > 0) {
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

	void GraphicManager::blend(BlendEquation blendEquation) {
		glEnable(GL_BLEND);

		switch(blendEquation) {
		case BlendEquation::Add:
			glBlendEquation(GL_FUNC_ADD);
			break;

		case BlendEquation::Subtract:
			glBlendEquation(GL_FUNC_SUBTRACT);
			break;

		case BlendEquation::ReverseSubtract:
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			break;

		case BlendEquation::Min:
#ifndef ANDROID
			glBlendEquation(GL_MIN);
#endif
			break;

		case BlendEquation::Max:
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

			if(!(usedTexturesSlots & mask)) {
				usedTexturesSlots |= mask;
				texturesUsed[i] = texture;
				textureName[i] = name;
				break;
			}
		}

		flags |= TextureAltered;
	}

	void GraphicManager::setVertexBuffer(unsigned int vertexBuffer) {
		if(this->vertexBuffer == vertexBuffer) return;

		this->vertexBuffer = vertexBuffer;
		flags |= VertexBufferAltered;
	}

	void GraphicManager::setIndexBuffer(unsigned int indexBuffer) {
		if(this->indexBuffer == indexBuffer) return;

		this->indexBuffer = indexBuffer;
		flags |= IndexBufferAltered;
	}

	void GraphicManager::setAttribute(AttributeOffset attributeOffset, int index, int mode, int offset, int stride) {
		//TODO Colocar um tipo nos atributos

		attribs[attributeOffset].attrib = index;
		attribs[attributeOffset].mode = mode;
		attribs[attributeOffset].offset = offset;
		attribs[attributeOffset].stride = stride;

		usedAttributes |= (1 << attributeOffset);

		flags |= AttributesAltered;
	}

	void GraphicManager::onResourceLoaded(const ResourceEvent& event) {
		if(event.type == "effect") {
			Effect* effect = (Effect*)event.resource;

			effect->finalizeInitialization();
		} else if(event.type == "model") {
			Model* model = (Model*)event.resource;

			model->uploadData(this);
		}
	}

	void GraphicManager::onResourceUnloaded(const ResourceEvent& event) {
		if(event.type == "model") {
			Model* model = (Model*)event.resource;

			model->unloadData(this);
		}
	}


	int getTarget(BufferType type) {
		switch(type) {
		case BufferType::VertexBuffer:
			return GL_ARRAY_BUFFER;

		case BufferType::IndexBuffer:
			return GL_ELEMENT_ARRAY_BUFFER;
		}

		return 0;
	}

	int getUsage(FrequencyAccess frequencyAccess, NatureAccess natureAccess) {
		if(frequencyAccess == FrequencyAccess::Stream && natureAccess == NatureAccess::Draw)
			return GL_STREAM_DRAW;

		if(frequencyAccess == FrequencyAccess::Stream && natureAccess == NatureAccess::Read)
			return GL_STREAM_READ;

		if(frequencyAccess == FrequencyAccess::Stream && natureAccess == NatureAccess::Copy)
			return GL_STREAM_COPY;

		if(frequencyAccess == FrequencyAccess::Static && natureAccess == NatureAccess::Draw)
			return GL_STATIC_DRAW;

		if(frequencyAccess == FrequencyAccess::Static && natureAccess == NatureAccess::Read)
			return GL_STATIC_READ;

		if(frequencyAccess == FrequencyAccess::Static && natureAccess == NatureAccess::Copy)
			return GL_STATIC_COPY;

		if(frequencyAccess == FrequencyAccess::Dynamic && natureAccess == NatureAccess::Draw)
			return GL_DYNAMIC_DRAW;

		if(frequencyAccess == FrequencyAccess::Dynamic && natureAccess == NatureAccess::Read)
			return GL_DYNAMIC_READ;

		if(frequencyAccess == FrequencyAccess::Dynamic && natureAccess == NatureAccess::Copy)
			return GL_DYNAMIC_COPY;

		return 0;
	}

	int getAccess(AccessType accessType) {
		if(accessType == AccessType::ReadOnly)
			return GL_READ_ONLY;

		if(accessType == AccessType::WriteOnly)
			return GL_WRITE_ONLY;

		if(accessType == AccessType::ReadWrite)
			return GL_READ_WRITE;

		return 0;
	}

	int BufferManager::createBuffer(int count, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess) {
		Buffer buffer;

		buffer.count = count;
		buffer.bufferType = bufferType;
		buffer.frequencyAccess = frequencyAccess;
		buffer.natureAccess = natureAccess;
		buffer.target = getTarget(bufferType);
		buffer.usage = getUsage(frequencyAccess, natureAccess);

		glGenBuffers(1, &buffer.bufferId);
		glBindBuffer(buffer.target, buffer.bufferId);
		glBufferData(buffer.target, buffer.count, 0, buffer.usage);

		return buffers.add(buffer);
	}

	void BufferManager::destroyBuffer(int handle) {
		if(!handle) return;

		Buffer& buffer = buffers.get(handle);

		glDeleteBuffers(1, &buffer.bufferId);

		buffers.remove(handle);
	}

	void* BufferManager::mapBuffer(int handle, AccessType accessType) {
		if(!handle) return 0;

		Buffer& buffer = buffers.get(handle);

		return glMapBuffer(buffer.target, getAccess(accessType));
	}

	void BufferManager::unmapBuffer(int handle) {
		if(!handle) return;

		Buffer& buffer = buffers.get(handle);

		glUnmapBuffer(buffer.target);
	}

}  // namespace engine
