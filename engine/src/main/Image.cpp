/*
 * Image.cpp
 *
 *  Created on: 01/04/2012
 *      Author: Marrony
 */
#include "Image.h"
#include "ResourceManager.h"
#include "Stream.h"
#include "ResourceStream.h"

namespace engine {

	Image::Image(const std::string& name, ResourceManager* manager) :
			Resource(name, manager), data(), width(width), height(height), format(format) {
	}

	Image::~Image() {
	}

	static int getDepthSize(ImageFormat format) {
		switch(format) {
		case ImageFormat::Rgb8:
			return 3;
		case ImageFormat::Rgba8:
			return 4;
		case ImageFormat::Rgba16Float:
			return 8;
		case ImageFormat::Rgba32Float:
			return 16;
		case ImageFormat::Depth:
			return 1;
		}

		return 0;
	}

	void Image::setData(int width, int height, ImageFormat format, const char* data) {
		this->width = width;
		this->height = height;
		this->format = format;

		int depthSize = getDepthSize(format);
		int length = width * height * depthSize;
		this->data.assign(data, data + length);
	}

	Image* ImageUtils::read(ResourceStream& stream, ResourceManager& manager) {
		stream.pushGroup("texture");

		std::string type = stream.readString("type");
		std::string name = stream.readString("name");
		int width = stream.readInt("width");
		int height = stream.readInt("height");
		ImageFormat format = (ImageFormat)stream.readInt("format");

		int depthSize = getDepthSize(format);

		int length = width * height * depthSize;
		std::vector<char> data(length);

		stream.readArray("pixels", data.data(), length);

		stream.popGroup();

		Image* image = new Image(name, &manager);
		image->width = width;
		image->height = height;
		image->format = format;
		image->data = data;

		return image;
	}

	void ImageUtils::write(ResourceStream& stream, ResourceManager& manager, Image* image) {
		int depthSize = getDepthSize(image->format);
		int size = image->width * image->height * depthSize;

		stream.pushGroup("texture");

		stream.writeString("type", image->getType().getName());
		stream.writeString("name", image->name);
		stream.writeInt("width", image->width);
		stream.writeInt("height", image->height);
		stream.writeInt("format", (int)image->format);
		stream.writeArray("pixels", image->data.data(), size);

		stream.popGroup();
	}

	Resource* ImageKey::loadResource(ResourceManager& manager) const {
		std::string textureName = getName();

		FileStream fileStream("resources/images/" + textureName + ".texture");
		ResourceBinStream resourceStream(fileStream);

		return ImageUtils::read(resourceStream, manager);
	}

} // namespace engine

