/*
 * Image.cpp
 *
 *  Created on: 01/04/2012
 *      Author: Marrony
 */
#include "Image.h"
#include "Exception.h"

#include <string.h>

namespace engine {

	Image::Image(int width, int height, int depth, const void* data) :
			data(0), width(width), height(height), depth(depth) {

		size_t size = width * height * depth;

		this->data = new char[size];
		memcpy(this->data, data, size * sizeof(char));
	}

	Image::~Image() {
		delete[] data;
	}

} // namespace engine

