/*
 * Image.h
 *
 *  Created on: 13/09/2011
 *      Author: marrony.neris
 */

#ifndef IMAGE_H_
#define IMAGE_H_

namespace engine {

	class Image {
	public:
		Image(int width, int height, int depth, const void* data);

		~Image();

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

	private:
		char* data;
		int width;
		int height;
		int depth;
	};

}  // namespace engine


#endif /* IMAGE_H_ */
