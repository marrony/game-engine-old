/*
 * OpenCL.cpp
 *
 *  Created on: 02/09/2011
 *      Author: marrony.neris
 */

#include "OpenCL.h"
#include "GraphicManager.h"

namespace engine {

	OpenCL::OpenCL() :
		opengl(0) {
		create();
	}

	OpenCL::OpenCL(GraphicManager* opengl) :
		opengl(opengl) {
		create();
	}

	OpenCL::~OpenCL() {
		destroy();
	}

	void OpenCL::createProgram(const char* source) {
		cl_int ret;
		cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &ret);
		ret = clBuildProgram(program, 1, &devices[0], NULL, NULL, NULL);

		cl_build_status buildStatus;
		size_t size;
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
		char* teste = new char[size+1];
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, size, teste, NULL);
		delete[] teste;
	}

	void OpenCL::create() {
		cl_uint ret_num_platforms;
		cl_uint num_devices;
		cl_int ret;

		ret = clGetPlatformIDs(1, &platformId, &ret_num_platforms);
		ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);

		devices.resize(num_devices);

		ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, num_devices, devices.data(), &num_devices);

		for(size_t i = 0; i < devices.size(); i++) {
			cl_device_type type;
			clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(type), &type, NULL);

			if(type == CL_DEVICE_TYPE_CPU) {
				//std::cout << "CL_DEVICE_TYPE_CPU" << std::endl;
			}

			if(type == CL_DEVICE_TYPE_GPU) {
				//std::cout << "CL_DEVICE_TYPE_GPU" << std::endl;
			}

			cl_bool compiler_available;
			clGetDeviceInfo(devices[i], CL_DEVICE_COMPILER_AVAILABLE, sizeof(compiler_available), &compiler_available, NULL);
			//std::cout << "CL_DEVICE_COMPILER_AVAILABLE " << compiler_available << std::endl;

			size_t size;
			clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS, 0, NULL, &size);

			char* extensions = new char[size+1];
			clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS, size, extensions, NULL);
			//std::cout << "CL_DEVICE_EXTENSIONS " << extensions << std::endl;

			delete[] extensions;
		}

		std::vector<cl_context_properties> properties;

		if(opengl != 0) {
			properties.push_back(CL_GL_CONTEXT_KHR);
			properties.push_back((cl_context_properties) wglGetCurrentContext());

			properties.push_back(CL_WGL_HDC_KHR);
			properties.push_back((cl_context_properties) wglGetCurrentDC());

			properties.push_back(CL_CONTEXT_PLATFORM);
			properties.push_back((cl_context_properties) platformId);
		}

		properties.push_back(0);

		context = clCreateContext(properties.data(), num_devices, devices.data(), NULL, NULL, &ret);
	}

	void OpenCL::destroy() {
		clReleaseContext(context);
	}

} /* namespace engine */
