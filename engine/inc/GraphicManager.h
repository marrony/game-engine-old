/*
 * GraphicManager.h
 *
 *  Created on: 06/03/2011
 *      Author: marrony
 */

#ifndef GRAPHICMANAGER_H_
#define GRAPHICMANAGER_H_

#include <map>
#include <string>

#include "definitions.h"
#include "GraphicManagerExtra.h"
#include "math/Vector.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Buffer.h"
#include "System.h"
#include "Material.h"
#include "Texture.h"
#include "Effect.h"

namespace engine {

	template<typename T>
	struct Resources {
		std::vector<T> resources;
		std::vector<int> free;

		int add(const T& resource) {
			if(!free.empty()) {
				int handle = free.back();
				free.pop_back();

				resources[handle] = resource;
				return handle + 1;
			}

			resources.push_back(resource);
			return resources.size();
		}

		void remove(int handle) {
			if(handle == 0) return;
			free.push_back(--handle);
		}

		T& get(int handle) {
			return resources[--handle];
		}
	};

	class TextureManager {
	public:
		struct Tex {
			unsigned int texId;
			int width;
			int height;
			int depth;
			TexType type;
			TextureFormat format;
		};

		Resources<Tex> textures;

		VIRTUAL int createTexture2D();
		VIRTUAL void destroyTexture(int handle);
		VIRTUAL void setTextureData(int handle, int width, int height, int depth, TextureFormat format, const void* data);
	};

	class BufferManager {
	public:
		struct Buffer {
			unsigned int bufferId;
			int count;
			int target;
			int usage;
			BufferType bufferType;
			FrequencyAccess frequencyAccess;
			NatureAccess natureAccess;
		};

		Resources<Buffer> buffers;

		VIRTUAL int createBuffer(int count, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess);
		VIRTUAL void destroyBuffer(int handle);
		VIRTUAL void* mapBuffer(int handle, AccessType accessType);
		VIRTUAL void unmapBuffer(int handle);
	};

	class GraphicManager : public ResourceListener {
		enum PendingFlags {
			VertexBufferAltered = 0x01,
			IndexBufferAltered  = 0x02,
			AttributesAltered   = 0x04,
			ConstantsAltered    = 0x08,
			ShaderAltered       = 0x10,
			TextureAltered      = 0x20,
			FramebufferAltered  = 0x40,
		};

		int width;
		int height;
		Framebuffer* framebuffer;
		Framebuffer* lastFramebuffer;
		Shader* shader;

		unsigned int vertexBuffer;
		unsigned int indexBuffer;

		struct Attribs {
			int attrib;
			int mode;
			int offset;
			int stride;
		};

		int flags;
		int lastUsedAttributes;
		int usedAttributes;
		Attribs attribs[AttributeOffset::MaxAttributeOffset];

		Texture* texturesUsed[16];
		std::string textureName[16];
		int usedTexturesSlots;
		int lastUsedTexturesSlots;

		ConstantContext context;
		std::vector<ConstantsEnabled> constants;

		void commitModifications();
	public:
		GraphicManager();
		VIRTUAL ~GraphicManager();

		virtual void swapBuffers() = 0;

		VIRTUAL void resize(int newWidth, int newHeight);

		VIRTUAL int getWidth();

		VIRTUAL int getHeight();

		VIRTUAL void cullFace(int face, int order);

		VIRTUAL void disableCullFace();

		VIRTUAL void enableAditiveBlend();

		VIRTUAL void blend(BlendEquation blendEquation);

		VIRTUAL void blend(int sfactor, int dfactor);

		VIRTUAL void disableBlend();

		VIRTUAL void scissorTest(const math::Vector4& scissor);

		VIRTUAL void disableScissor();

		VIRTUAL void depthTest(int depthFunc);

		VIRTUAL void disableDepthTest();

		VIRTUAL void clear(const math::Vector4& color, float depth);

		VIRTUAL void clear(const math::Vector4& color);

		VIRTUAL void bindFramebuffer(Framebuffer* const newFramebuffer);

		VIRTUAL void setShader(Shader* const newShader);

		VIRTUAL void enableDepthWrite();

		VIRTUAL void disableDepthWrite();

		VIRTUAL void enableColorWrite();

		VIRTUAL void disableColorWrite();

		VIRTUAL void enableTexture();

		VIRTUAL void disableTexture();

		VIRTUAL void drawIndex(int start, int end, int count, int offset);

		VIRTUAL void drawIndex(const unsigned short* index, unsigned int count);

		VIRTUAL void bindTexture(const std::string& name, class Texture* texture);

		VIRTUAL void setVertexBuffer(unsigned int vertexBuffer);
		VIRTUAL void setIndexBuffer(unsigned int indexBuffer);

		VIRTUAL void setAttribute(AttributeOffset attributeOffset, int index, int mode, int offset, int stride);

		VIRTUAL void setConstant(ConstantContext& context, std::vector<ConstantsEnabled>& constants) {
			this->context = context;
			this->constants = constants;

			flags |= ConstantsAltered;
		}

		virtual void onResourceLoaded(const ResourceEvent& event);
		virtual void onResourceUnloaded(const ResourceEvent& event);

		TextureManager textureManager;
		BufferManager bufferManager;
	};

}

#endif /* GRAPHICMANAGER_H_ */
