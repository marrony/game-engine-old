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
#include "GeometryData.h"
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

		if(flags & VertexBufferAltered) {
			if(vertexBuffer && vertexBuffer->needUpdate())
				updateBuffer(vertexBuffer);
			else
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getHandle());
		}

		if(flags & IndexBufferAltered) {
			if(indexBuffer && indexBuffer->needUpdate())
				updateBuffer(indexBuffer);
			else
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getHandle());
		}

		if(flags & TextureAltered) {
			for(int unit = 0; unit < 16; ++unit) {
				int mask = (1 << unit);

				if(usedTexturesSlots & mask) {
					shader->getConstant(textureName[unit])->setValue(unit);

					if(texturesUsed[unit]->needUpdate())
						updateTexture(texturesUsed[unit]);

					glActiveTexture(GL_TEXTURE0 + unit);
#ifndef ANDROID
					//glClientActiveTexture(GL_TEXTURE0 + unit);
#endif
					glBindTexture(GL_TEXTURE_2D, texturesUsed[unit]->getHandle());

				}
			}

			for(int unit = 0; unit < 16; ++unit) {
				int mask = (1 << unit);

				if(!(usedTexturesSlots & mask) && (lastUsedTexturesSlots & mask)) {
					glActiveTexture(GL_TEXTURE0 + unit);
#ifndef ANDROID
					//glClientActiveTexture(GL_TEXTURE0 + unit);
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
					size_t bonesCount = context.model->skeleton->getBonesCount();

					if(context.model->hasAnimation) {
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

	void GraphicManager::updateTexture(Texture* texture) {
		if(!texture->getHandle()) {
			GLuint texId;
			glGenTextures(1, &texId);

			texture->setHandle(texId);
		}

		int textureUnit = 15;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &textureUnit);

		GLenum type;

		switch(texture->getTextureType()) {
			case TextureType::Texture2D:
				type = GL_TEXTURE_2D;
				break;

			case TextureType::Texture3D:
				type = GL_TEXTURE_3D;
				break;

			case TextureType::TextureCube:
				type = GL_TEXTURE_CUBE_MAP;
				break;
		}

		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(type, texture->getHandle());

		Image* image = texture->getImage();

		const std::vector<char>& data = image->getData();

		int pixelType = GL_UNSIGNED_BYTE;

		int pixelFormat;
		int internalFormat;

		switch(image->getFormat()) {
			case ImageFormat::Rgb8:
				pixelFormat = GL_RGB;
				internalFormat = GL_RGB;
				break;

			case ImageFormat::Rgba8:
				pixelFormat = GL_RGBA;
				internalFormat = GL_RGBA;
				break;

#ifndef ANDROID
			case ImageFormat::Rgba16Float:
				pixelType = GL_FLOAT;
				pixelFormat = GL_RGBA;
				internalFormat = GL_RGBA16F;
				break;

			case ImageFormat::Rgba32Float:
				pixelType = GL_FLOAT;
				pixelFormat = GL_RGBA;
				internalFormat = GL_RGBA32F;
				break;
#endif

			default:
				throw Exception("Unknow depth");
		}

		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(type, 0, internalFormat, image->getWidth(), image->getHeight(), 0, pixelFormat, pixelType, data.data());

		glBindTexture(type, 0);

		texture->setUpdated();
	}

	Texture* GraphicManager::getTexture(Image* image) {
		auto texture = images.find(image);

		if(texture != images.end())
			return texture->second;

		Texture* tex = new Texture;
		tex->setImage(image);
		updateTexture(tex);
		images[image] = tex;

		return tex;
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

	void GraphicManager::updateBuffer(Buffer* buffer) {
		int handle = buffer->getHandle();

		if(!handle) {
			GLuint bufferId;
			glGenBuffers(1, &bufferId);

			handle = bufferId;
			buffer->setHandle(handle);
		}

		GLsizeiptr count = buffer->getSize();
		GLenum target = getTarget(buffer->getBufferType());
		GLenum usage = getUsage(buffer->getFrequencyAccess(), buffer->getNatureAccess());
		const GLvoid* data = buffer->getData();

		glBindBuffer(target, handle);
		glBufferData(target, count, data, usage);

		buffer->setUpdated();
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

	void GraphicManager::bindTexture(const std::string& name, Image* image) {
		for(int i = 0; i < 16; i++) {
			int mask = (1 << i);

			if(!(usedTexturesSlots & mask)) {
				usedTexturesSlots |= mask;
				texturesUsed[i] = getTexture(image);
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

	void* GraphicManager::mapBuffer(Buffer* buffer, AccessType accessType) {
		updateBuffer(buffer);

		GLenum target = getTarget(buffer->getBufferType());
		GLenum access = getAccess(accessType);

		return glMapBuffer(target, access);
	}

	void GraphicManager::unmapBuffer(Buffer* buffer) {
		GLenum target = getTarget(buffer->getBufferType());
		glUnmapBuffer(target);
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
		}
	}

	void GraphicManager::onResourceUnloaded(const ResourceEvent& event) {
	}

	void GraphicManager::getBuffers(Mesh* geometry, Buffer** indexBuffer, Buffer** vertexBuffer) {
		auto entry = buffers.find(geometry);

		if(entry != buffers.end()) {
			*indexBuffer = entry->second.indexBuffer;
			*vertexBuffer = entry->second.vertexBuffer;
		} else {
			Buffer* vBuffer = new Buffer(geometry->elementsPerVertex * geometry->position.size() * sizeof(float), BufferType::VertexBuffer,
					FrequencyAccess::Static, NatureAccess::Draw);

			Buffer* iBuffer = new Buffer(geometry->indices.size() * sizeof(unsigned short), BufferType::IndexBuffer,
					FrequencyAccess::Static, NatureAccess::Draw);

			void* indexPtr = mapBuffer(iBuffer, AccessType::WriteOnly);
			memcpy(indexPtr, geometry->indices.data(), geometry->indices.size() * sizeof(unsigned short));
			unmapBuffer(iBuffer);

			float* vertexPtr = (float*) mapBuffer(vBuffer, AccessType::WriteOnly);

			for(size_t i = 0; i < geometry->position.size(); i++) {
				/*if(!geometry->boneIds.empty() && !geometry->weights.empty() && !geometry->bindPose.empty()) {
					math::Vector3 pos(0, 0, 0);

					if(geometry->boneIds[i].x > 0)
						pos += geometry->bindPose[(int) geometry->boneIds[i].x] * geometry->position[i] * geometry->weights[i].x;

					if(geometry->boneIds[i].y > 0)
						pos += geometry->bindPose[(int) geometry->boneIds[i].y] * geometry->position[i] * geometry->weights[i].y;

					*vertexPtr++ = pos.x;
					*vertexPtr++ = pos.y;
					*vertexPtr++ = pos.z;

					*vertexPtr++ = geometry->boneIds[i].x;
					*vertexPtr++ = geometry->boneIds[i].y;
					*vertexPtr++ = geometry->boneIds[i].z;
					*vertexPtr++ = geometry->boneIds[i].w;

					*vertexPtr++ = geometry->weights[i].x;
					*vertexPtr++ = geometry->weights[i].y;
					*vertexPtr++ = geometry->weights[i].z;
					*vertexPtr++ = geometry->weights[i].w;
				} else*/ {
					*vertexPtr++ = geometry->position[i].x;
					*vertexPtr++ = geometry->position[i].y;
					*vertexPtr++ = geometry->position[i].z;

					*vertexPtr++ = 0;
					*vertexPtr++ = -1;
					*vertexPtr++ = -1;
					*vertexPtr++ = -1;

					*vertexPtr++ = 1;
					*vertexPtr++ = 0;
					*vertexPtr++ = 0;
					*vertexPtr++ = 0;
				}

				if(!geometry->normal.empty()) {
					*vertexPtr++ = geometry->normal[i].x;
					*vertexPtr++ = geometry->normal[i].y;
					*vertexPtr++ = geometry->normal[i].z;
				}

				if(!geometry->sTangent.empty() && !geometry->tTangent.empty()) {
					*vertexPtr++ = geometry->sTangent[i].x;
					*vertexPtr++ = geometry->sTangent[i].y;
					*vertexPtr++ = geometry->sTangent[i].z;

					*vertexPtr++ = geometry->tTangent[i].x;
					*vertexPtr++ = geometry->tTangent[i].y;
					*vertexPtr++ = geometry->tTangent[i].z;
				}

				if(!geometry->color.empty()) {
					*vertexPtr++ = geometry->color[i].x;
					*vertexPtr++ = geometry->color[i].y;
					*vertexPtr++ = geometry->color[i].z;
				}

				if(!geometry->texCoord.empty()) {
					*vertexPtr++ = geometry->texCoord[i].x;
					*vertexPtr++ = geometry->texCoord[i].y;
				}
			}

			unmapBuffer(vBuffer);

			buffers.insert({geometry, {iBuffer, vBuffer}});

			*indexBuffer = iBuffer;
			*vertexBuffer = vBuffer;
		}
	}

}  // namespace engine
