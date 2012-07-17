/*
 * TGALoader.cpp
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */

#include "ResourcePlugin.h"
#include "ResourceCompiler.h"
#include "MemoryTexture.h"
#include "Image.h"
#include "FileUtil.h"

#include <fstream>
#include <algorithm>

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

/*
 A TGA file has a header that consists of 12 fields. These are:

 # 0x00 - 0x00 - id (uchar)
 # 0x01 - 0x01 - colour map type (uchar)
 # 0x02 - 0x02 - image type (uchar)
 # 0x03 - 0x04 - colour map first entry (short int)
 # 0x05 - 0x06 - colour map length (short int)
 # 0x07 - 0x07 - colour map depth (uchar)
 # 0x08 - 0x09 - horizontal origin (short int)
 # 0x0a - 0x0b - vertical origin (short int)
 # 0x0c - 0x0d - width (short int)
 # 0x0e - 0x0f - height (short int)
 # 0x10 - 0x10 - pixel depth (uchar)
 # 0x11 - 0x11 - image descriptor (uchar)

 Some possible values for the image type are:

 # 1 - colour map image
 # 2 - RGB(A) uncompressed
 # 3 - greyscale uncompressed
 # 9 - greyscale RLE (compressed)
 # 10 - RGB(A) RLE (compressed)
 */
struct TGAHeader {
	unsigned char id;
	unsigned char colour_map_type;
	unsigned char image_type;
	short int colour_map_first_entry;
	short int colour_map_length;
	unsigned char colour_map_depth;
	short int horizontal_origin;
	short int vertical_origin;
	short int width;
	short int height;
	unsigned char pixel_depth;
	unsigned char image_descriptor;
};

class TGALoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;
public:
	virtual ~TGALoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		this->manager = manager;

		compiler->registerLoader(this, "TGA Loader", "tga");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		std::ifstream stream(fileName, std::ios::binary);

		if(!stream.good())
			return;

		TGAHeader header;
		stream.read((char*)&header.id, sizeof(header.id));
		stream.read((char*)&header.colour_map_type, sizeof(header.colour_map_type));
		stream.read((char*)&header.image_type, sizeof(header.image_type));
		stream.read((char*)&header.colour_map_first_entry, sizeof(header.colour_map_first_entry));
		stream.read((char*)&header.colour_map_length, sizeof(header.colour_map_length));
		stream.read((char*)&header.colour_map_depth, sizeof(header.colour_map_depth));
		stream.read((char*)&header.horizontal_origin, sizeof(header.horizontal_origin));
		stream.read((char*)&header.vertical_origin, sizeof(header.vertical_origin));
		stream.read((char*)&header.width, sizeof(header.width));
		stream.read((char*)&header.height, sizeof(header.height));
		stream.read((char*)&header.pixel_depth, sizeof(header.pixel_depth));
		stream.read((char*)&header.image_descriptor, sizeof(header.image_descriptor));

		if(header.image_type != 2 && header.image_type != 3 && header.image_type != 10) {
			return;
		}

		if(header.pixel_depth != 8 && header.pixel_depth != 24 && header.pixel_depth != 32) {
			return;
		}

		if(header.width == 0 || header.height == 0) {
			return;
		}

		unsigned char pixel_size = header.pixel_depth / 8;
		unsigned int nbytes = header.width * header.height * pixel_size;
		unsigned char* dados = new unsigned char[nbytes];
		if(dados == 0) {
			return;
		}

		if(header.image_type == 10) {
			unsigned int bytesToProcess = nbytes;

			unsigned char* dst = dados;

			while(bytesToProcess > 0) {
				int byteRead = stream.get();

				int bytesToRead = (byteRead & 0x7f) + 1;

				bytesToProcess -= bytesToRead * pixel_size;

				if(byteRead & 0x80) {
					unsigned char pixels[8];

					stream.read((char*)pixels, sizeof(unsigned char) * pixel_size);

					do {
						memcpy(dst, pixels, pixel_size);
						dst += pixel_size;
					} while(--bytesToRead);

				} else {
					bytesToRead *= pixel_size;
					stream.read((char*)dst, sizeof(unsigned char) * bytesToRead);
					dst += bytesToRead;
				}
			}
		} else {
			stream.read((char*)dados, sizeof(unsigned char)* nbytes);
		}

		if(pixel_size >= 3) {
			for(unsigned int i = 0; i < nbytes; i += pixel_size)
				std::swap(dados[i + 0], dados[i + 2]);
		}

		Image* texture = 0;

		switch(pixel_size) {
		case 1:
			break;

		case 3:
			texture = new Image(header.width, header.height, 3, dados);
			break;

		case 4:
			texture = new Image(header.width, header.height, 4, dados);
			break;

		default:
			break;
		}

		delete[] dados;

		std::string outputName = file::getPath(fileName) + "/" + file::getFilename(fileName) + ".texture";
		FileStream fileStream(outputName);
		ResourceBinStream resourceStream(fileStream);
		TextureUtils::write(resourceStream, *manager, texture);
	}
};

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new TGALoader;
}
