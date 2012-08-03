/*
 * Texture.cpp
 *
 *  Created on: 04/07/2011
 *      Author: marrony
 */

#include "Texture.h"
#include "Image.h"
#include "ResourceManager.h"
#include "GraphicManager.h"
#include "TextureManager.h"
#include "Exception.h"
#include "Stream.h"

namespace engine {

	void Texture::cleanData() {
		if(data) {
			delete[] data;
			data = 0;
		}
	}

	Texture::Texture(const std::string& name, ResourceManager* manager) :
			Resource(name, manager), handle(0), dirty(true) {
	}

	Texture::~Texture() {
		cleanData();
	}

	Type Texture::getType() const {
		return Type("texture");
	}

	void Texture::markUploaded() {
		uploaded = true;
		cleanData();
	}

	void Texture::setHandle(int handle) {
		this->handle = handle;
	}

	int Texture::getHandle() const {
		return handle;
	}

	const void* Texture::getData() const {
		return data;
	}

	int Texture::getWidth() const {
		return width;
	}

	int Texture::getHeight() const {
		return height;
	}

	int Texture::getDepth() const {
		return depth;
	}

	void Texture::setData(int width, int height, int depth, const void* data) {
		this->width = width;
		this->height = height;
		this->depth = depth;

		size_t size = width * height * depth;

		this->data = new char[size];
		memcpy(this->data, data, size * sizeof(char));
	}

	void Texture::initialize(GraphicManager* graphicManager) {
//		handle = graphicManager->textureManager.createTexture2D();
//
//		TextureFormat format = TextureFormat::Rgb8;
//
//		graphicManager->textureManager.setTextureData(handle, width, height, depth, format, data);
//
//		markUploaded();
	}

	void Texture::finalize(GraphicManager* graphicManager) {
//		graphicManager->textureManager.destroyTexture(handle);
	}

	void* TextureUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		stream.pushGroup("texture");

		std::string type = stream.readString("type");
		std::string name = stream.readString("name");
		int width = stream.readInt("width");
		int height = stream.readInt("height");
		int depth = stream.readInt("depth");

		size_t length = width * height * depth;
		char* data = new char[length];

		stream.readArray("pixels", data, length);

		stream.popGroup();

		Texture* texture = new Texture(name, &manager);
		texture->width = width;
		texture->height = height;
		texture->depth = depth;
		texture->data = data;
		texture->uploaded = false;

		return texture;
	}

	void TextureUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Texture* texture = dynamic_cast<Texture*>((Texture*) instance);

		if(!texture)
			throw Exception("Resource is not a texture");

		int width = texture->getWidth();
		int height = texture->getHeight();
		int depth = texture->getDepth();
		int size = width * height * depth;
		const char* data = (const char*)texture->getData();

		stream.pushGroup("texture");

		stream.writeString("type", texture->getType().getName());
		stream.writeString("name", texture->name);
		stream.writeInt("width", width);
		stream.writeInt("height", height);
		stream.writeInt("depth", depth);
		stream.writeArray("pixels", data, size);

		stream.popGroup();
	}

	Resource* TextureKey::loadResource(ResourceManager& manager) const {
		std::string textureName = getName();

		FileStream fileStream("resources/images/" + textureName + ".texture");
		ResourceBinStream resourceStream(fileStream);

		Texture* texture = (Texture*)TextureUtils::read(resourceStream, manager, 0);

		return texture;
	}

} // namespace engine
