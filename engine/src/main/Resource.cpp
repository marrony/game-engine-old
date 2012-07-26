/*
 * Resource.cpp
 *
 *  Created on: 03/05/2012
 *      Author: Marrony
 */
#include "Resource.h"
#include "Stream.h"

#include <sstream>

namespace engine {

	ResourceBinStream::ResourceBinStream(Stream& stream) :
			stream(stream) {
	}

	ResourceBinStream::~ResourceBinStream() {
	}

	void ResourceBinStream::pushGroup(const char* label) {
	}

	void ResourceBinStream::popGroup() {
	}

	void ResourceBinStream::writeString(const char* label, const std::string& value) {
		writeString(value);
	}

	void ResourceBinStream::writeString(const std::string& value) {
		int size = value.size();

		stream.write(&size, sizeof(int));
		stream.write(value.c_str(), size);
	}

	void ResourceBinStream::writeInt(const char* label, int value) {
		writeInt(value);
	}

	void ResourceBinStream::writeInt(int value) {
		stream.write(&value, sizeof(int));
	}

	void ResourceBinStream::writeShort(const char* label, short value) {
		writeShort(value);
	}

	void ResourceBinStream::writeShort(short value) {
		stream.write(&value, sizeof(short));
	}

	void ResourceBinStream::writeByte(const char* label, char value) {
		writeByte(value);
	}

	void ResourceBinStream::writeByte(char value) {
		stream.write(&value, sizeof(char));
	}

	void ResourceBinStream::writeFloat(const char* label, float value) {
		writeFloat(value);
	}

	void ResourceBinStream::writeFloat(float value) {
		stream.write(&value, sizeof(float));
	}

	void ResourceBinStream::writeArray(const char* label, const int* array, size_t count) {
		stream.write(array, sizeof(int) * count);
	}

	void ResourceBinStream::writeArray(const char* label, const short* array, size_t count) {
		stream.write(array, sizeof(short) * count);
	}

	void ResourceBinStream::writeArray(const char* label, const char* array, size_t count) {
		stream.write(array, sizeof(char) * count);
	}

	void ResourceBinStream::writeArray(const char* label, const float* array, size_t count) {
		stream.write(array, sizeof(float) * count);
	}

	std::string ResourceBinStream::readString(const char* label) {
		return readString();
	}

	std::string ResourceBinStream::readString() {
		char* bytes = 0;

		try {
			int size;
			stream.read(&size, sizeof(int));

			bytes = new char[size + 1];
			stream.read(bytes, size);
			bytes[size] = 0;

			std::string str(bytes);

			delete[] bytes;

			return str;
		} catch(...) {
			delete[] bytes;
		}

		return std::string();
	}

	int ResourceBinStream::readInt(const char* label) {
		return readInt();
	}

	int ResourceBinStream::readInt() {
		int value;

		stream.read(&value, sizeof(int));

		return value;
	}

	short ResourceBinStream::readShort(const char* label) {
		return readShort();
	}

	short ResourceBinStream::readShort() {
		short value;

		stream.read(&value, sizeof(short));

		return value;
	}

	char ResourceBinStream::readByte(const char* label) {
		return readByte();
	}

	char ResourceBinStream::readByte() {
		char value;

		stream.read(&value, sizeof(char));

		return value;
	}

	float ResourceBinStream::readFloat(const char* label) {
		return readFloat();
	}

	float ResourceBinStream::readFloat() {
		float value;

		stream.read(&value, sizeof(float));

		return value;
	}

	void ResourceBinStream::readArray(const char* label, int* array, size_t count) {
		stream.read(array, sizeof(int) * count);
	}

	void ResourceBinStream::readArray(const char* label, short* array, size_t count) {
		stream.read(array, sizeof(short) * count);
	}

	void ResourceBinStream::readArray(const char* label, char* array, size_t count) {
		stream.read(array, sizeof(char) * count);
	}

	void ResourceBinStream::readArray(const char* label, float* array, size_t count) {
		stream.read(array, sizeof(float) * count);
	}

	///////////////////////////////////////////////////////////////////////////////////////

	ResourceXmlStream::ResourceXmlStream(TiXmlDocument& document) :
			document(document) {
	}

	ResourceXmlStream::~ResourceXmlStream() {
	}

	void ResourceXmlStream::pushGroup(const char* label) {
		if(nodes.empty())
			nodes.push(&document);

		TiXmlNode* node = getTag(label);

		if(!node)
			node = createTag(label);

		nodes.push(node);
	}

	void ResourceXmlStream::popGroup() {
		nodes.pop();
	}

	TiXmlElement* ResourceXmlStream::createTag(const char* label) {
		TiXmlNode* root = nodes.top();
		return root->InsertEndChild(TiXmlElement(label))->ToElement();
	}

	TiXmlElement* ResourceXmlStream::getTag(const char* label) {
		TiXmlNode* root = nodes.top();
		return root->FirstChildElement(label);
	}

	void ResourceXmlStream::writeString(const char* label, const std::string& value) {
		createTag(label)->InsertEndChild(TiXmlText(value.c_str()));
	}

	void ResourceXmlStream::writeInt(const char* label, int value) {
		char str[32];

		snprintf(str, sizeof(str), "%d", value);

		createTag(label)->InsertEndChild(TiXmlText(str));
	}

	void ResourceXmlStream::writeShort(const char* label, short value) {
		char str[32];

		snprintf(str, sizeof(str), "%d", value);

		createTag(label)->InsertEndChild(TiXmlText(str));
	}

	void ResourceXmlStream::writeByte(const char* label, char value) {
		char str[32];

		snprintf(str, sizeof(str), "%d", value);

		createTag(label)->InsertEndChild(TiXmlText(str));
	}

	void ResourceXmlStream::writeFloat(const char* label, float value) {
		char str[32];

		snprintf(str, sizeof(str), "%f", value);

		createTag(label)->InsertEndChild(TiXmlText(str));
	}

	void ResourceXmlStream::writeArray(const char* label, const int* array, size_t count) {
		std::stringstream stream;

		for(size_t i = 0; i < count; i++) {
			int value = array[i] & 0xffffffff;

			stream << std::hex << value;

			if(i < count - 1)
				stream << ' ';
		}

		std::string str = stream.str();

		createTag(label)->InsertEndChild(TiXmlText(str.c_str()));
	}

	void ResourceXmlStream::writeArray(const char* label, const short* array, size_t count) {
		std::stringstream stream;

		for(size_t i = 0; i < count; i++) {
			int value = (int) array[i] & 0xffff;

			stream << std::hex << value;

			if(i < count - 1)
				stream << ' ';
		}

		std::string str = stream.str();

		createTag(label)->InsertEndChild(TiXmlText(str.c_str()));
	}

	void ResourceXmlStream::writeArray(const char* label, const char* array, size_t count) {
		std::stringstream stream;

		for(size_t i = 0; i < count; i++) {
			int value = (int) array[i] & 0xff;

			stream << std::hex << value;

			if(i < count - 1)
				stream << ' ';
		}

		std::string str = stream.str();

		createTag(label)->InsertEndChild(TiXmlText(str.c_str()));
	}

	void ResourceXmlStream::writeArray(const char* label, const float* array, size_t count) {
		std::stringstream stream;

		for(size_t i = 0; i < count; i++) {
			int value = * (int*) (array + i);

			stream << array[i] << ':' << std::hex << value;

			if(i < count - 1)
				stream << ' ';
		}

		std::string str = stream.str();

		createTag(label)->InsertEndChild(TiXmlText(str.c_str()));
	}

	std::string ResourceXmlStream::readString(const char* label) {
		TiXmlNode* node = getTag(label);

		return node->ToElement()->GetText();
	}

	int ResourceXmlStream::readInt(const char* label) {
		TiXmlNode* node = getTag(label);

		int value;

		sscanf(node->ToElement()->GetText(), "%d", &value);

		return value;
	}

	short ResourceXmlStream::readShort(const char* label) {
		TiXmlNode* node = getTag(label);

		int value;

		sscanf(node->ToElement()->GetText(), "%d", &value);

		return value & 0xffff;
	}

	char ResourceXmlStream::readByte(const char* label) {
		TiXmlNode* node = getTag(label);

		int value;

		sscanf(node->ToElement()->GetText(), "%d", &value);

		return value & 0xff;
	}

	float ResourceXmlStream::readFloat(const char* label) {
		TiXmlNode* node = getTag(label);

		float value;

		sscanf(node->ToElement()->GetText(), "%f", &value);

		return value;
	}

	void ResourceXmlStream::readArray(const char* label, int* array, size_t count) {
		TiXmlNode* node = getTag(label);

		const char* value = node->ToElement()->GetText();

		for(size_t i = 0; i < count; i++) {
			int nbytes = 0;

			sscanf(value, "%08X%n", array + i, &nbytes);

			value += nbytes;
		}
	}

	void ResourceXmlStream::readArray(const char* label, short* array, size_t count) {
		TiXmlNode* node = getTag(label);

		const char* value = node->ToElement()->GetText();

		for(size_t i = 0; i < count; i++) {
			int nbytes = 0;

			int valueParssed;

			sscanf(value, "%x%n", &valueParssed, &nbytes);

			array[i] = valueParssed & 0xffff;

			value += nbytes;
		}
	}

	void ResourceXmlStream::readArray(const char* label, char* array, size_t count) {
		TiXmlNode* node = getTag(label);

		const char* value = node->ToElement()->GetText();

		for(size_t i = 0; i < count; i++) {
			int nbytes = 0;

			int valueParssed;

			sscanf(value, "%x%n", &valueParssed, &nbytes);

			array[i] = valueParssed & 0xff;

			value += nbytes;
		}
	}

	void ResourceXmlStream::readArray(const char* label, float* array, size_t count) {
		TiXmlNode* node = getTag(label);

		const char* value = node->ToElement()->GetText();

		for(size_t i = 0; i < count; i++) {
			int nbytes = 0;

			float ignore;

			if(sscanf(value, "%f:%x%n", &ignore, array + i, &nbytes) == 2) {
				value += nbytes;
			} else if(sscanf(value, "%f%n", array + i, &nbytes) == 1) {
				value += nbytes;
			}
		}
	}

} // namespace engine

