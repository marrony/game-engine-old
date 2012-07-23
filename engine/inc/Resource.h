/*
 * Resource.h
 *
 *  Created on: 20/03/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "Type.h"
#include "ResourceStream.h"
#include "Stream.h"

#include "tinyxml.h"

#include <stack>
#include <functional>

namespace engine {

	class ResourceManager;

	class Resource {
	public:
		virtual ~Resource() {
		}

		virtual Type getType() const = 0;

		std::string getName() const {
			return name;
		}

		ResourceManager* getManager() {
			return manager;
		}
	protected:
		Resource(const std::string& name, ResourceManager* manager) :
				name(name), manager(manager) {
		}

		std::string name;
		ResourceManager* manager;
	};


	class ResourceBinStream : public ResourceStream {
	public:
		ResourceBinStream(Stream& stream);
		virtual ~ResourceBinStream();

		virtual void pushGroup(const char* label);
		virtual void popGroup();

		virtual void writeString(const char* label, const std::string& value);
		virtual void writeString(const std::string& value);

		virtual void writeInt(const char* label, int value);
		virtual void writeInt(int value);

		virtual void writeShort(const char* label, short value);
		virtual void writeShort(short value);

		virtual void writeByte(const char* label, char value);
		virtual void writeByte(char value);

		virtual void writeFloat(const char* label, float value);
		virtual void writeFloat(float value);

		virtual void writeArray(const char* label, const int* array, size_t count);
		virtual void writeArray(const char* label, const short* array, size_t count);
		virtual void writeArray(const char* label, const char* array, size_t count);
		virtual void writeArray(const char* label, const float* array, size_t count);

		virtual std::string readString(const char* label);
		virtual std::string readString();

		virtual int readInt(const char* label);
		virtual int readInt();

		virtual short readShort(const char* label);
		virtual short readShort();

		virtual char readByte(const char* label);
		virtual char readByte();

		virtual float readFloat(const char* label);
		virtual float readFloat();

		virtual void readArray(const char* label, int* array, size_t count);
		virtual void readArray(const char* label, short* array, size_t count);
		virtual void readArray(const char* label, char* array, size_t count);
		virtual void readArray(const char* label, float* array, size_t count);
	private:
		Stream& stream;
	};

	class ResourceXmlStream : public ResourceStream {
		TiXmlDocument& document;
		std::stack<TiXmlNode*> nodes;

		TiXmlElement* createTag(const char* label);
		TiXmlElement* getTag(const char* label);
	public:
		ResourceXmlStream(TiXmlDocument& document);
		virtual ~ResourceXmlStream();

		virtual void pushGroup(const char* label);
		virtual void popGroup();

		virtual void writeString(const char* label, const std::string& value);
		virtual void writeInt(const char* label, int value);
		virtual void writeShort(const char* label, short value);
		virtual void writeByte(const char* label, char value);
		virtual void writeFloat(const char* label, float value);
		virtual void writeArray(const char* label, const int* array, size_t count);
		virtual void writeArray(const char* label, const short* array, size_t count);
		virtual void writeArray(const char* label, const char* array, size_t count);
		virtual void writeArray(const char* label, const float* array, size_t count);

		virtual std::string readString(const char* label);
		virtual int readInt(const char* label);
		virtual short readShort(const char* label);
		virtual char readByte(const char* label);
		virtual float readFloat(const char* label);
		virtual void readArray(const char* label, int* array, size_t count);
		virtual void readArray(const char* label, short* array, size_t count);
		virtual void readArray(const char* label, char* array, size_t count);
		virtual void readArray(const char* label, float* array, size_t count);
	};

	struct ResourceEvent {
		std::string type;
	};

	struct ResourceListener {
		virtual ~ResourceListener() {}

		virtual void onResourceLoaded(const ResourceEvent& event) = 0;
		virtual void onResourceUnloaded(const ResourceEvent& event) = 0;
	};

	class ResourceKey {
		std::string name;
	public:
		ResourceKey(const std::string& name) {
			size_t index = name.find('/');

			if(index != std::string::npos)
				this->name = name.substr(index + 1);
			else
				this->name = name;
		}

		virtual ~ResourceKey() {}

		virtual std::string getName() const {
			return name;
		}

		virtual std::string getKeyName() const = 0;

		virtual Resource* loadResource(ResourceManager& manager) const = 0;
	};

} // namespace engine

#endif /* RESOURCE_H_ */
