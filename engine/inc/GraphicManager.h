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
#include "TextureManager.h"

#include "Resources.h"

namespace engine {

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

		Texture* texturesUsed[16];
		std::string textureName[16];
		int usedTexturesSlots;
		int lastUsedTexturesSlots;

		ConstantContext context;
		std::vector<ConstantsEnabled> constants;

		void commitModifications();
		void updateTexture(Texture* texture);
		void updateBuffer(Buffer* buffer);
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

		VIRTUAL void setVertexBuffer(Buffer* vertexBuffer);
		VIRTUAL void setIndexBuffer(Buffer* indexBuffer);

		VIRTUAL void* mapBuffer(Buffer* buffer, AccessType accessType);
		VIRTUAL void unmapBuffer(Buffer* buffer);

		VIRTUAL void setAttribute(AttributeOffset attributeOffset, int index, int mode, int offset, int stride);

		VIRTUAL void setConstant(ConstantContext& context, std::vector<ConstantsEnabled>& constants) {
			this->context = context;
			this->constants = constants;

			flags |= ConstantsAltered;
		}

		virtual void onResourceLoaded(const ResourceEvent& event);
		virtual void onResourceUnloaded(const ResourceEvent& event);
	};

}

#endif /* GRAPHICMANAGER_H_ */
