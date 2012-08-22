/*
 * VoxelRenderer.h
 *
 *  Created on: 31/08/2011
 *      Author: marrony.neris
 */

#ifndef VOXELRENDERER_H_
#define VOXELRENDERER_H_

#include "Shader.h"
#include "MyImpls.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "RenderEntity.h"
#include "Geometry.h"
#include "GameEntity.h"

#include "SparseVoxelOctreeBuilder.h"
#include "SparseVoxelOctree.h"
#include "ObjSceneLoader.h"
#include "ms3d/MS3DModelLoader.h"
#include "Model.h"

#include <fstream>
#include <iostream>

#include <CL/opencl.h>

#define STRINGFY(x) #x

class VoxelRenderer : public engine::Renderer {
	engine::Texture* openclTexture;
	engine::Texture* depthTexture;
	cl_kernel kernel;
	cl_command_queue queue;
	cl_mem tex;
	engine::SparseVoxelOctree* svo;
	bool initialized;

	engine::ShaderProgram* getShaderProgramVoxel() {
		std::ifstream fragment("shaders/voxel.frag.c");
		engine::ShaderSource* fragmentShader = new engine::ShaderSource(fragment);

		std::ifstream vertex("shaders/voxel.vert.c");
		engine::ShaderSource* vertexShader = new engine::ShaderSource(vertex);

		engine::ShaderProgram* shaderVoxel = new engine::ShaderProgram(vertexShader, fragmentShader, 0);
		shaderVoxel->link();

		return shaderVoxel;
	}

	void drawQuad(engine::GraphicManager* graphicManager, engine::Attribute* myVertex, engine::Attribute* myTexCoord) {
		const engine::math::Vector2 tex[4] = {
			engine::math::Vector2(0, 0),
			engine::math::Vector2(0, 1),
			engine::math::Vector2(1, 1),
			engine::math::Vector2(1, 0),
		};

		myVertex->setValue(tex, 4);
		myTexCoord->setValue(tex, 4);

		unsigned short index[] = {0, 1, 2, 0, 2, 3};
		graphicManager->drawIndex(index, sizeof(index) / sizeof(unsigned short));
	}

public:
	VoxelRenderer() {
		openclTexture = 0;
		depthTexture = 0;
		kernel = 0;
		queue = 0;
		tex = 0;
		svo = 0;
		initialized = false;
	}

	virtual void render(engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
		if(!initialized) {
			initialized = true;

			depthTexture = new engine::Texture(new engine::TextureUnsignedNormalizedFormat(engine::ImageFormat::Depth, 24), 512, 512);
			openclTexture = new engine::Texture(new engine::TextureUnsignedNormalizedFormat(engine::ImageFormat::RGBA, 16), 512, 512);

			cl_int ret;

			const char* helloWorldSource = STRINGFY(
			__kernel void teste(__write_only image2d_t tex, int i) {
				int x = get_global_id(0);
				int y = get_global_id(1);

				int2 coords = (int2)(x, y);

				float4 val;

				if(x == i) {
					val = (float4)(0, 1, 0, 0);
				} else {
					val = (float4)(1, 0, 0, 0);
				}

				write_imagef(tex, coords, val);
			});

//			cl_program program = clCreateProgramWithSource(engine::win32::contextx, 1, &helloWorldSource, NULL, &ret);
//			ret = clBuildProgram(program, 1, &engine::win32::device, NULL, NULL, NULL);
//
//			cl_build_status buildStatus;
//			size_t size;
//			clGetProgramBuildInfo(program, engine::win32::device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
//			char* teste = new char[size+1];
//			clGetProgramBuildInfo(program, engine::win32::device, CL_PROGRAM_BUILD_LOG, size, teste, NULL);
//			delete[] teste;
//
//			glBindTexture(GL_TEXTURE_2D, openclTexture->getTextureId());
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//			glBindTexture(GL_TEXTURE_2D, 0);
//
//			tex = clCreateFromGLTexture2D(engine::win32::contextx, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, openclTexture->getTextureId(), &ret);
//
//			kernel = clCreateKernel(program, "teste", &ret);
//
//			clSetKernelArg(kernel, 0, sizeof(cl_mem), &tex);
//
//			queue = clCreateCommandQueue(engine::win32::contextx, engine::win32::device, 0, &ret);
		}

		const engine::Camera* const camera = sceneManager->getActualCamera();
		std::vector<engine::Geometry*> geometries = sceneManager->getVisibleGeometries(camera->getFrustum());

		std::cout << "entidades visiveis: " << geometries.size() << std::endl;

//		if(passShader) {
//			static cl_int x = 0;
//			x++;
//			x %= 512;
//			clSetKernelArg(kernel, 1, sizeof(cl_int), &x);
//
//			clEnqueueAcquireGLObjects(queue, 1, &tex, 0, NULL, NULL);
//
//			size_t global_work_offset[2] = {512, 512};
//			size_t local_work_size[2] = {16, 16};
//			clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_offset, local_work_size, 0, NULL, NULL);
//
//			clEnqueueReleaseGLObjects(queue, 1, &tex, 0, NULL, NULL);
//
////			clFinish(queue);
////			clFlush(queue);
//
//			applyFilter(graphicManager, passShader, openclTexture);
//		}

		if(!svo) {
			engine::Entity* entity = sceneManager->findEntity("mesh01");
			engine::Geometry* geo = entity->getComponent<engine::Geometry>();

			engine::SparseVoxelOctreeBuilder svoBuilder(geo->getModel());
			svo = svoBuilder.build(5);
			engine::math::Vector3 v0(+1, -1, -1);
			engine::math::Vector3 v1(-1, -1, +1);
			engine::math::Vector3 v2(+1, +1, +1);
			//svo = svoBuilder.buildDebug(7, v0, v1, v2);
		}

		static engine::ShaderProgram* prog = 0;

		if(!prog) {
			prog = getShaderProgramVoxel();
		}

		if(prog) {
			GLint viewport[4] = {0};
			glGetIntegerv(GL_VIEWPORT, viewport);
			size_t width = viewport[2] - viewport[0];
			size_t height = viewport[3] - viewport[1];
			std::vector<engine::math::Vector3> colorBuffer(width * height, engine::math::Vector3(1, 1, 1));
			std::vector<float> depthBuffer(width * height, 1.0);
			svo->render(camera->getFrustum(), colorBuffer, depthBuffer, width, height);

			glBindTexture(GL_TEXTURE_2D, openclTexture->getTextureId());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, colorBuffer.data());
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindTexture(GL_TEXTURE_2D, depthTexture->getTextureId());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthBuffer.data());
			glBindTexture(GL_TEXTURE_2D, 0);

			engine::math::Matrix4 ortho = engine::math::Matrix4::orthoMatrix(0, 1, 0, 1);

			prog->bind();

			prog->getConstant("myProjectionMatrix")->setValue(ortho);

			prog->getConstant("pixelWidth")->setValue(1.0f / width);
			prog->getConstant("pixelHeight")->setValue(1.0f / height);
			prog->getConstant("fillColor")->setValue(engine::math::Vector4(0.5, 0.5, 0.5, 1.0));
			prog->getConstant("maxDim")->setValue(1.0f);
			prog->getConstant("horRes")->setValue(width);
			prog->getConstant("vertRes")->setValue(height);

			prog->bindTextureUnit(0, "colorSampler", openclTexture);
			prog->bindTextureUnit(1, "depthSampler", depthTexture);

			engine::Attribute* myVertex = prog->getAttribute("myVertex");
			engine::Attribute* myTexCoord = prog->getAttribute("myTexCoord");

			drawQuad(graphicManager, myVertex, myTexCoord);

			prog->unbindTextures();
			prog->unbind();

//			applyFilter(graphicManager, prog, depthTexture);
		}
	}
};


#endif /* VOXELRENDERER_H_ */
