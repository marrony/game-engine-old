/*
 * Texture.cpp
 *
 *  Created on: 04/07/2011
 *      Author: marrony
 */

#include "Texture.h"
#include "Image.h"
#include "ResourceManager.h"
#include "Exception.h"
#include "Stream.h"

namespace engine {

	void* TextureUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		stream.pushGroup("texture");

//		std::string type = stream.readString("type");
//		std::string name = stream.readString("name");
		int width = stream.readInt("width");
		int height = stream.readInt("height");
		int depth = stream.readInt("depth");

		size_t length = width * height * depth;
		char* data = new char[length];

		stream.readArray("pixels", data, length);

		stream.popGroup();

		Image* texture = new Image(width, height, depth, data);

		delete[] data;

		return texture;
	}

	void TextureUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Image* texture = dynamic_cast<Image*>((Image*) instance);

		if(!texture)
			throw Exception("Resource is not a texture");

		int width = texture->getWidth();
		int height = texture->getHeight();
		int depth = texture->getDepth();
		int size = width * height * depth;
		const char* data = (const char*)texture->getData();

		stream.pushGroup("texture");

//		stream.writeString("type", texture->getType().getName());
//		stream.writeString("name", texture->name);
		stream.writeInt("width", width);
		stream.writeInt("height", height);
		stream.writeInt("depth", depth);
		stream.writeArray("pixels", data, size);

		stream.popGroup();
	}

	Resource* TextureKey::loadResource(ResourceManager& manager) const {
		std::string textureName = getName();

		Texture* texture = new Texture(textureName, &manager);

		FileStream fileStream("resources/images/" + textureName + ".texture");
		ResourceBinStream resourceStream(fileStream);

		Image* image = (Image*)TextureUtils::read(resourceStream, manager, 0);

		TextureEvent event;
		event.type = "texture";
		event.image = image;
		event.texture = texture;

		manager.dispatchLoadedEvent(event);

		delete image;

		return texture;
	}

} // namespace engine
