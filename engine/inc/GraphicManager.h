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
#include "math/Vector.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Buffer.h"
#include "HardwareBuffer.h"
#include "System.h"
#include "Material.h"
#include "Texture.h"
#include "Effect.h"

namespace engine {

	struct Blend {
		enum Equation {
			ADD,              // Result = (Source Color * Source Blend) + (Destination Color * Destination Blend)
			SUBTRACT,         // Result = (Source Color * Source Blend) - (Destination Color * Destination Blend)
			REVERSE_SUBTRACT, // Result = (Destination Color * Destination Blend) - (Source Color * Source Blend)
			MIN,              // Result = min( (Source Color * Source Blend), (Destination Color * Destination Blend) )
			MAX               // Result = max( (Source Color * Source Blend), (Destination Color * Destination Blend) )
		};
	};

	class GraphicManager : public ResourceListener {
		template<typename T>
		struct Resources {
			std::vector<T> resources;
			std::vector<size_t> free;

			size_t add(const T& resource) {
				if(!free.empty()) {
					size_t handle = free.back();
					free.pop_back();

					resources[handle] = resource;
					return handle + 1;
				}

				resources.push_back(resource);
				return resources.size();
			}

			void remove(size_t handle) {
				if(handle == 0) return;
				free.push_back(--handle);
			}

			T& get(size_t handle) {
				return resources[--handle];
			}
		};

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

		Buffer* vertexBuffer;
		Buffer* indexBuffer;

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

		VIRTUAL void blend(Blend::Equation blendEquation);

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

		VIRTUAL void setVertexBuffer(Buffer* vertexBuffer);
		VIRTUAL void setIndexBuffer(Buffer* indexBuffer);

		VIRTUAL void setAttribute(AttributeOffset attributeOffset, int index, int mode, int offset, int stride);

		ConstantContext context;
		std::vector<ConstantsEnabled> constants;
		VIRTUAL void setConstant(ConstantContext& context, std::vector<ConstantsEnabled>& constants) {
			this->context = context;
			this->constants = constants;

			flags |= ConstantsAltered;
		}

		enum TexType {
			Texture2D = GL_TEXTURE_2D,
			Texture3D = GL_TEXTURE_3D,
			TextureCube = GL_TEXTURE_CUBE_MAP
		};

		virtual void onResourceLoaded(const ResourceEvent& event);
		virtual void onResourceUnloaded(const ResourceEvent& event);

		struct Tex {
			unsigned int texId;
			int width;
			int height;
			int depth;
			TexType type;
			TextureFormat format;
		};

		Resources<Tex> textures0;
		Texture* textures[16];
		std::string textureName[16];
		int usedTextures;
		int lastUsedTextures;

		VIRTUAL int createTexture2D();
		VIRTUAL void destroyTexture(int handle);
		VIRTUAL void setTextureData(int handle, int width, int height, int depth, TextureFormat format, const void* data);

		VIRTUAL Buffer* createBuffer(int count, BufferType bufferType, FrequencyAccess frequencyAccess, NatureAccess natureAccess);
	};

}

#endif /* GRAPHICMANAGER_H_ */
