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
#include "System.h"
#include "Material.h"

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


	class GraphicManager {
		enum PendingFlags {
			VertexBufferAltered = 0x01,
			IndexBufferAltered  = 0x02,
			AttributesAltered   = 0x04,
			ShaderAltered       = 0x08,
			TextureAltered      = 0x10,
			FramebufferAltered  = 0x20,
		};

		int width;
		int height;
		Framebuffer* framebuffer;
		Framebuffer* lastFramebuffer;
		Shader* shader;

		Buffer* vertexBuffer;
		Buffer* indexBuffer;

		struct Attribs {
			Attribute* attribute;
			int mode;
			int offset;
			int stride;
		};

		int flags;
		int lastUsedAttributes;
		int usedAttributes;
		Attribs attribs[AttributeOffset::MaxAttributeOffset];

		class Texture* textures[16];
		std::string textureName[16];
		int lastUsedTextures;

		std::map<std::string, class Texture*> samplers;

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

		VIRTUAL void unbindShader(); //TODO remover

		VIRTUAL void enableDepthWrite();

		VIRTUAL void disableDepthWrite();

		VIRTUAL void enableColorWrite();

		VIRTUAL void disableColorWrite();

		VIRTUAL void enableTexture();

		VIRTUAL void disableTexture();

		VIRTUAL void drawIndex(Buffer* indexBuffer, int start, int end, int count, int offset);

		VIRTUAL void drawIndex(const unsigned short* index, unsigned int count);

		VIRTUAL void bindTexture(const std::string& name, class Texture* texture);

		VIRTUAL void addSampler(const std::string& name, class Texture* sampler); //TODO remover

		VIRTUAL void removeSamplers();

		VIRTUAL void setVertexBuffer(Buffer* vertexBuffer);
		VIRTUAL void setIndexBuffer(Buffer* indexBuffer);

		VIRTUAL void setAttribute(int index, Attribute* attr, int mode, int offset, int stride);
	};

}

#endif /* GRAPHICMANAGER_H_ */
