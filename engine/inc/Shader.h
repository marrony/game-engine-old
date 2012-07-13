/*
 * Shader.h
 *
 *  Created on: 01/03/2011
 *      Author: marrony
 */

#ifndef SHADER_H_
#define SHADER_H_

#include "math/Vector.h"
#include "math/Matrix.h"

#include <string>
#include <set>
#include <map>

namespace engine {

	class Buffer;

	class Shader;

	class Constant {
		Shader* program;
		int uniformLocation;
	public:
		Constant(Shader* program, unsigned int uniformLocation, const std::string& name);

		void setValue(int value);
		void setValue(unsigned int value);
		void setValue(float value);
		void setValue(const math::Vector2& value);
		void setValue(const math::Vector3& value);
		void setValue(const math::Vector4& value);
		void setValue(const math::Matrix4& matrix);
		void setValue(const math::Matrix3& matrix);
		void setValue(const math::Matrix3 matrix[], size_t count);
		void setValue(const math::Matrix4 matrix[], size_t count);
	};

	class Attribute {
		Shader* program;
		int attribLocation;

		void enableAttribute();
	public:
		Attribute(Shader* program, unsigned int attribLocation, const std::string& name);

		void setValue(const math::Vector2* value, int count);
		void setValue(const math::Vector3* value, int count);
		void setValue(const math::Vector4* value, int count);
		void setValue(int mode, int offset, int stride, Buffer* value);
		void setValue(int mode, int offset, int stride);

		void enable();
		void disable();

		int getIndex() {
			return attribLocation;
		}
	};

	enum ShaderType {
		VertexShader,
		FragmentShader,
		GeometryShader
	};

	class Source {
		int shaderId;
		std::string source;
		bool compiled;
	public:
		Source(std::istream& stream);
		Source(const std::string& _source);
		~Source();

		int getId();
		void compile(ShaderType type);
		std::string getSource() const;
	};

	/**
	 * Sampler contem as informações de filtro das texturas (nearest, linear, bilinear) e etc.
	 */
	class Sampler {
	public:
		Sampler(const std::string& name) :
				name(name) {
		}
	private:
		std::string name;
	};

	class Shader {
		static const int MAX_TEXTURE_UNITS = 16;

		Source* vertexShader;
		Source* fragmentShader;
		Source* geometryShader;
		int programId;

		std::set<Attribute*> attribsEnabled;
		std::map<std::string, Attribute*> attributes;
		std::map<std::string, Constant*> constants;
		std::map<std::string, Sampler> samplers;

		void disableAttribs();
	public:
		Shader(Source* vertex, Source* fragment, Source* geometry);
		~Shader();

		void bind();
		void unbind();

		Source* getFragmentShader();
		Source* getVertexShader();
		Source* getGeometryShader();

		void link();

		Constant* getConstant(const std::string& name) const;
		Attribute* getAttribute(const std::string& name) const;
		int getAttributeIndex(const std::string& name) const;
		Sampler* getSampler(const std::string& name) {
			std::map<std::string, Sampler>::iterator ite = samplers.find(name);

			if(ite != samplers.end())
				return &ite->second;

			return 0;
		}

		std::map<std::string, Sampler> getSamplers() {
			return samplers;
		}

		void enableAttribute(Attribute* parameter);
	};
}

#endif /* SHADER_H_ */
