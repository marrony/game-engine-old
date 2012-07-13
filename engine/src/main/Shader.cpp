/*
 * Shader.cpp
 *
 *  Created on: 04/07/2011
 *      Author: marrony
 */

#include "Shader.h"
#include "Buffer.h"
#include "Exception.h"
#include "FileUtil.h"
#include "Texture.h"

#include <set>
#include <map>

#include "System.h"

namespace engine {

	Constant::Constant(Shader* program, unsigned int uniformLocation, const std::string& name) :
			program(program), uniformLocation(uniformLocation) {
	}

	void Constant::setValue(int value) {
		if(uniformLocation != -1) {
			glUniform1i(uniformLocation, value);
		}
	}

	void Constant::setValue(unsigned int value) {
		if(uniformLocation != -1) {
#ifdef ANDROID
			glUniform1i(uniformLocation, (int) value);
#else
			glUniform1ui(uniformLocation, value);
#endif
		}
	}

	void Constant::setValue(float value) {
		if(uniformLocation != -1) {
			glUniform1f(uniformLocation, value);
		}
	}

	void Constant::setValue(const engine::math::Vector2& value) {
		if(uniformLocation != -1) {
			glUniform2f(uniformLocation, value.x, value.y);
		}
	}

	void Constant::setValue(const engine::math::Vector3& value) {
		if(uniformLocation != -1) {
			glUniform3f(uniformLocation, value.x, value.y, value.z);
		}
	}

	void Constant::setValue(const engine::math::Vector4& value) {
		if(uniformLocation != -1) {
			glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
		}
	}

	void Constant::setValue(const engine::math::Matrix4& matrix) {
		if(uniformLocation != -1) {
			glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, matrix.getAsArray());
		}
	}

	void Constant::setValue(const engine::math::Matrix3& matrix) {
		if(uniformLocation != -1) {
			glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, matrix.getAsArray());
		}
	}

	void Constant::setValue(const math::Matrix3 matrix[], size_t count) {
		if(uniformLocation != -1) {
			glUniformMatrix3fv(uniformLocation, count, GL_FALSE, matrix[0].getAsArray());
		}
	}

	void Constant::setValue(const math::Matrix4 matrix[], size_t count) {
		if(uniformLocation != -1) {
			glUniformMatrix4fv(uniformLocation, count, GL_FALSE, matrix[0].getAsArray());
		}
	}

	Attribute::Attribute(Shader* program, unsigned int attribLocation, const std::string& name) :
			program(program), attribLocation(attribLocation) {
	}

	void Attribute::setValue(int mode, int offset, int stride, engine::Buffer* buffer) {
		if(buffer->isOk()) {
			if(attribLocation != -1) {
				enableAttribute();

				void* pointer = (char*) buffer->getPointer() + offset;

				glVertexAttribPointer(attribLocation, mode, GL_FLOAT, GL_FALSE, stride, pointer);
			}
		}
	}

	void Attribute::setValue(int mode, int offset, int stride) {
		if(attribLocation != -1) {
			glVertexAttribPointer(attribLocation, mode, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		}
	}

	void Attribute::setValue(const engine::math::Vector2* value, int count) {
		if(attribLocation != -1) {
			enableAttribute();
			glVertexAttribPointer(attribLocation, 2, GL_FLOAT, GL_FALSE, 0, value);
		}
	}

	void Attribute::setValue(const engine::math::Vector3* value, int count) {
		if(attribLocation != -1) {
			enableAttribute();
			glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 0, value);
		}
	}

	void Attribute::setValue(const engine::math::Vector4* value, int count) {
		if(attribLocation != -1) {
			enableAttribute();
			glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, 0, value);
		}
	}

	void Attribute::enable() {
		glEnableVertexAttribArray(attribLocation);
	}

	void Attribute::disable() {
		glDisableVertexAttribArray(attribLocation);
	}

	void Attribute::enableAttribute() {
		program->enableAttribute(this);
	}

	Source::Source(std::istream& stream) :
			shaderId(0), source(), compiled(false) {
		source = engine::file::loadFromStream(stream);
	}

	Source::Source(const std::string& _source) :
			shaderId(0), source(_source), compiled(false) {
	}

	Source::~Source() {
		if(shaderId)
			glDeleteShader(shaderId);
	}

	int Source::getId() {
		return shaderId;
	}

	void Source::compile(ShaderType shaderType) {
		if(!compiled) {
			int type;

			switch(shaderType) {
			case VertexShader:
				type = GL_VERTEX_SHADER;
				break;

			case FragmentShader:
				type = GL_FRAGMENT_SHADER;
				break;

#ifndef ANDROID
			case GeometryShader:
				type = GL_GEOMETRY_SHADER;
				break;
#endif
			}

			shaderId = glCreateShader(type);

			const GLchar* sourcePtr = source.data();

			glShaderSource(shaderId, 1, &sourcePtr, 0);

			glCompileShader(shaderId);

			int status = 0;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);

			if(!status) {
				int lenght;
				char log[1024];

				glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &lenght);
				glGetShaderInfoLog(shaderId, 1024, &lenght, log);

				std::cout << "Compile status" << std::endl << log << std::endl << std::endl;

				std::cout << source << std::endl;
			}

			compiled = (status == GL_TRUE);
		}
	}

	std::string Source::getSource() const {
		return source;
	}

	void Shader::disableAttribs() {
		for(std::set<Attribute*>::iterator i = attribsEnabled.begin(); i != attribsEnabled.end(); i++) {
			(*i)->disable();
		}
		attribsEnabled.clear();
	}

	Shader::Shader(Source* vertex, Source* fragment, Source* geometry) :
			vertexShader(vertex), fragmentShader(fragment), geometryShader(geometry), programId(0) {

//		memset(textures, 0, sizeof(textures));
	}

	Shader::~Shader() {
		delete vertexShader;
		delete fragmentShader;
		delete geometryShader;

		std::map<std::string, Attribute*>::iterator attribute = attributes.begin();
		while(attribute != attributes.end()) {
			delete attribute->second;

			attribute++;
		}

		std::map<std::string, Constant*>::iterator constant = constants.begin();
		while(constant != constants.end()) {
			delete constant->second;

			constant++;
		}

		if(programId)
			glDeleteProgram(programId);
	}

	Source* Shader::getFragmentShader() {
		return fragmentShader;
	}

	Source* Shader::getVertexShader() {
		return vertexShader;
	}

	Source* Shader::getGeometryShader() {
		return geometryShader;
	}

	void Shader::link() {
		programId = glCreateProgram();

		if(vertexShader != 0) {
			vertexShader->compile(VertexShader);
			glAttachShader(programId, vertexShader->getId());
		}

		if(fragmentShader != 0) {
			fragmentShader->compile(FragmentShader);
			glAttachShader(programId, fragmentShader->getId());
		}

		if(geometryShader != 0) {
			geometryShader->compile(GeometryShader);
			glAttachShader(programId, geometryShader->getId());
		}

		glLinkProgram(programId);

		int status = 0;
		glGetProgramiv(programId, GL_LINK_STATUS, &status);

		if(status == GL_FALSE) {
			int lenght;
			char log[1024];

			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &lenght);
			glGetProgramInfoLog(programId, 1024, &lenght, log);

			std::cout << "Link status" << std::endl << log << std::endl << std::endl;

			if(status == GL_FALSE) {
				throw engine::Exception("");
			}
		}

		int numAttribs = 0;
		glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &numAttribs);

		std::cout << "Attributes" << std::endl;
		for(int i = 0; i < numAttribs; i++) {
			GLchar name[256] = { 0 };
			GLint size = 0;
			GLenum type = 0;

			glGetActiveAttrib(programId, i, sizeof(name), 0, &size, &type, name);

			std::cout << name << std::endl;

			int attribLocation = glGetAttribLocation(programId, name);
			attributes[name] = new Attribute(this, attribLocation, name);
		}

		std::cout << std::endl;

		int numUniforms = 0;
		glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numUniforms);

		std::cout << "Uniforms" << std::endl;
		for(int i = 0; i < numUniforms; i++) {
			GLchar name[256] = { 0 };
			GLint size = 0;
			GLenum type = 0;

			glGetActiveUniform(programId, i, sizeof(name), 0, &size, &type, name);

			if(size > 1) {
				char* str = strchr(name, '[');

				if(str)
					*str = 0;
			}

			bool isSampler =
#ifndef ANDROID
					type == GL_SAMPLER_1D ||
					type == GL_SAMPLER_3D ||
					type == GL_SAMPLER_1D_SHADOW ||
					type == GL_SAMPLER_2D_SHADOW ||
#endif
					type == GL_SAMPLER_2D ||
					type == GL_SAMPLER_CUBE;

			std::cout << name << std::endl;

			int uniformLocation = glGetUniformLocation(programId, name);

			Constant* constant = new Constant(this, uniformLocation, name);
			constants[name] = constant;

			if(isSampler)
				samplers.insert(std::make_pair(name, Sampler(name)));
		}

		std::cout << std::endl;
	}

	void Shader::bind() {
		disableAttribs();
		glUseProgram(programId);
	}

	void Shader::unbind() {
		disableAttribs();
		glUseProgram(0);
	}

	static Constant NULL_CONSTANT(0, -1, "null");
	static Attribute NULL_ATTRIBUTE(0, -1, "null");

	Constant* Shader::getConstant(const std::string& name) const {
		std::map<std::string, Constant*>::const_iterator c = constants.find(name);

		if(c == constants.end())
			return &NULL_CONSTANT;

		return c->second;
	}

	Attribute* Shader::getAttribute(const std::string& name) const {
		std::map<std::string, Attribute*>::const_iterator a = attributes.find(name);

		if(a == attributes.end())
			return &NULL_ATTRIBUTE;

		return a->second;
	}

	void Shader::enableAttribute(Attribute* parameter) {
		parameter->enable();
		attribsEnabled.insert(parameter);
	}

} // namespace engine

