/*
 * Image.h
 *
 *  Created on: 13/09/2011
 *      Author: marrony.neris
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "Resource.h"

#include <vector>
#include <string>

namespace engine {

	class ResourceManager;

	enum class ImageFormat : unsigned char {
		None,
		Rgb8,
		Rgba8,
		Rgba16Float,
		Rgba32Float,
		Depth
	};

	class Image : public Resource {
	public:
		Image(const std::string& name, ResourceManager* manager);

		virtual ~Image();

		virtual Type getType() const {
			return Type("image");
		}

		const std::vector<char>& getData() const {
			return data;
		}

		void setData(int width, int heigth, ImageFormat format, const char* data);

		int getWidth() const {
			return width;
		}

		int getHeight() const {
			return height;
		}

		ImageFormat getFormat() const {
			return format;
		}

	private:
		friend class ImageUtils;

		std::vector<char> data;
		ImageFormat format;
		int width;
		int height;
	};

	struct ImageUtils {
		static Image* read(ResourceStream&, class ResourceManager&);
		static void write(ResourceStream&, class ResourceManager&, Image*);
	};

	class ImageKey : public ResourceKey {
	public:
		ImageKey(const std::string& name) : ResourceKey(name) {}

		virtual std::string getKeyName() const {
			return "texture/" + getName();
		}

		virtual Resource* loadResource(class ResourceManager& manager) const;
	};

}  // namespace engine


#endif /* IMAGE_H_ */
