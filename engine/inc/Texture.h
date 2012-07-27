/*
 * Texture.h
 *
 *  Created on: 22/10/2010
 *      Author: marrony.neris
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"

namespace engine {

	class Image;

	enum class TextureFormat : unsigned char {
		None,
		Rgb8,
		Rgba8,
		Rgba16Float,
		Rgba32Float,
		Depth
	};

	class Texture : public Resource {
		int width;
		int height;
		int depth;
		bool uploaded;
		char* data;
		int handle;

		void cleanData() {
			if(data) {
				delete[] data;
				data = 0;
			}
		}
	public:
		Texture(const std::string& name, ResourceManager* manager) :
				Resource(name, manager), handle(0) {
		}

		virtual ~Texture() {
			cleanData();
		}

		virtual Type getType() const {
			return Type("texture");
		}

		void markUploaded() {
			uploaded = true;
			cleanData();
		}

		void setHandle(int handle) {
			this->handle = handle;
		}

		int getHandle() const {
			return handle;
		}

		const void* getData() const {
			return data;
		}

		int getWidth() const {
			return width;
		}

		int getHeight() const {
			return height;
		}

		int getDepth() const {
			return depth;
		}

		void setData(int width, int height, int depth, const void* data) {
			this->width = width;
			this->height = height;
			this->depth = depth;

			size_t size = width * height * depth;

			this->data = new char[size];
			memcpy(this->data, data, size * sizeof(char));
		}

		friend class TextureUtils;
	};

	struct TextureUtils {
		static void* read(ResourceStream&, class ResourceManager&, void*);
		static void write(ResourceStream&, class ResourceManager&, void*);
	};

	class TextureKey : public ResourceKey {
	public:
		TextureKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "texture/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

}  // namespace engine

#endif /* TEXTURE_H_ */
