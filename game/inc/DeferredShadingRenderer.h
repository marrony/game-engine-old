/*
 * DeferredShadingRenderer.h
 *
 *  Created on: 24/11/2010
 *      Author: marrony.neris
 */

#ifndef DEFERREDSHADINGRENDERER_H_
#define DEFERREDSHADINGRENDERER_H_

#include "Engine.h"
#include "GL/ShaderImpl.h"
#include "GL/TextureImpl.h"
#include "MyImpls.h"
#include "GL/FramebufferImpl.h"
#include <fstream>
#include <map>

class DeferredShading : implements engine::Renderer {
	static const int COLOR0_BUFFER = 0;
	static const int COLOR1_BUFFER = 1;
	static const int COLOR2_BUFFER = 2;
	static const int DEPTH_BUFFER = 3;

	int width, height;
	bool frameBufferCreated;
	engine::Pointer<engine::ShaderProgram> shaderProgramDepthPass;
	engine::Pointer<engine::ShaderProgram> shaderProgramMaterialPass;
	engine::Pointer<engine::ShaderProgram> shaderProgramLightingPass;
	engine::Pointer<engine::ShaderProgram> shaderProgramShadowPass;

	engine::Pointer<engine::ShaderProgram> blurShader;
	engine::Pointer<engine::ShaderProgram> passShader;

	engine::Pointer<engine::Framebuffer> gbuffer;
	engine::Texture* gtextures[4];

	engine::Pointer<engine::Framebuffer> lightbuffer;
	engine::Texture* lighttexture;

	engine::Pointer<engine::Framebuffer> filterFramebuffer;
	engine::Texture* filtertexture;

	engine::Pointer<engine::Framebuffer> shadowFramebuffer;

	static const int VERTEX = 0x01;
	static const int NORMAL = 0x02;
	static const int COLOR = 0x04;
	static const int TEXTURE = 0x08;

	void renderEntity(engine::Entity* actor, engine::ShaderProgram* shaderProgram, int drawFlags) {
		engine::mesh::Mesh* mesh = actor->getMesh();

		if(drawFlags & VERTEX) {
			engine::Buffer *vertexBuffer = mesh->getVertexBuffer();
			if(vertexBuffer->isOk()) {
				vertexBuffer->bind();
				shaderProgram->setParameter("myVertex", vertexBuffer);
				vertexBuffer->unbind();
				CHECK_ERROR();
			}
		}

		if(drawFlags & NORMAL) {
			engine::Buffer *normalBuffer = mesh->getNormalBuffer();
			if(normalBuffer->isOk()) {
				normalBuffer->bind();
				shaderProgram->setParameter("myNormal", normalBuffer);
				normalBuffer->unbind();
				CHECK_ERROR();
			}

			engine::Buffer *tangentBuffer = mesh->getTangentBuffer();
			if(tangentBuffer->isOk()) {
				tangentBuffer->bind();
				shaderProgram->setParameter("myTangent", tangentBuffer);
				tangentBuffer->unbind();
				CHECK_ERROR();
			}

			engine::Buffer *binormalBuffer = mesh->getBinormalBuffer();
			if(binormalBuffer->isOk()) {
				binormalBuffer->bind();
				shaderProgram->setParameter("myBinormal", binormalBuffer);
				binormalBuffer->unbind();
				CHECK_ERROR();
			}
		}

		if(drawFlags & COLOR) {
			shaderProgram->setParameter("myDiffuseColor", engine::math::Vector3(1.0, 1.0, 1.0));
			CHECK_ERROR();
			shaderProgram->setParameter("mySpecularColor", engine::math::Vector3(0.1, 0.1, 0.8));
			CHECK_ERROR();

			engine::Buffer *colorBuffer = mesh->getColorBuffer();
			if(colorBuffer->isOk()) {
				colorBuffer->bind();
				shaderProgram->setParameter("myColor", colorBuffer);
				colorBuffer->unbind();
				CHECK_ERROR();
			}
		}

		if(drawFlags & TEXTURE) {
			glEnable(GL_TEXTURE_2D);
			engine::Material* material = actor->getMaterial();
			for(int index = 0; index < material->getTextureCount(); index++) {
				engine::Buffer *textureBuffer = mesh->getTextureBuffer();

				if(textureBuffer->isOk()) {
					char buffer[256];

					engine::Texture *texture = material->getTexture(index);
					texture->bind(index);
					CHECK_ERROR();

					textureBuffer->bind();
					shaderProgram->setParameter("myTexCoord", textureBuffer);
					CHECK_ERROR();
					textureBuffer->unbind();

					sprintf(buffer, "myTextureId%d", index);
					shaderProgram->setParameter(buffer, index);

					CHECK_ERROR();
				}
			}
		}

		engine::Buffer *indexBuffer = mesh->getIndexBuffer();
		if(indexBuffer->isOk()) {
			indexBuffer->bind();
			glDrawElements(GL_TRIANGLES, indexBuffer->getCount(), GL_UNSIGNED_INT, indexBuffer->getPointer());
			CHECK_ERROR();

			indexBuffer->unbind();
		}

		if(drawFlags & TEXTURE) {
			engine::Material* material = actor->getMaterial();
			for(int index = 0; index < material->getTextureCount(); index++) {
				engine::Texture *texture = material->getTexture(index);
				texture->unbind(index);
			}

			glDisable(GL_TEXTURE_2D);
		}

		CHECK_ERROR();
	}

	engine::ShaderProgram* getShaderProgramDepthPass() {
		if(!shaderProgramDepthPass) {
			std::ifstream vertex("shaders/depthVertex.vert.c");
			VertexShader* vertexShader = new VertexShader(vertex);

			std::ifstream fragment("shaders/depthFragment.frag.c");
			FragmentShader* fragmentShader = new FragmentShader(fragment);

			shaderProgramDepthPass = new ShaderProgramImpl(vertexShader, fragmentShader, 0);
			shaderProgramDepthPass->link();
		}

		return shaderProgramDepthPass.get();
	}

	engine::ShaderProgram* getShaderProgramMaterialPass() {
		if(!shaderProgramMaterialPass) {
			std::ifstream vertex("shaders/vertex.vert.c");
			VertexShader* vertexShader = new VertexShader(vertex);

			std::ifstream fragment("shaders/fragment.frag.c");
			FragmentShader* fragmentShader = new FragmentShader(fragment);

			GeometryShader* geometryShader = 0;//new GeometryShader("");

			shaderProgramMaterialPass = new ShaderProgramImpl(vertexShader, fragmentShader, geometryShader);
			shaderProgramMaterialPass->link();
		}

		return shaderProgramMaterialPass.get();
	}

	engine::ShaderProgram* getShaderProgramLigthingState() {
		if(!shaderProgramLightingPass) {
			std::ifstream vertex("shaders/lightingVertex.vert.c");
			VertexShader* vertexShader = new VertexShader(vertex);

			std::ifstream fragment("shaders/lightingFragment.frag.c");
			FragmentShader* fragmentShader = new FragmentShader(fragment);

			GeometryShader* geometryShader = 0;//new GeometryShader("");

			shaderProgramLightingPass = new ShaderProgramImpl(vertexShader, fragmentShader, geometryShader);
			shaderProgramLightingPass->link();
		}

		return shaderProgramLightingPass.get();
	}

	engine::ShaderProgram* getShaderProgramShadowPass() {
		if(!shaderProgramShadowPass) {
			std::ifstream vertex("shaders/shadowVertex.vert.c");
			VertexShader* vertexShader = new VertexShader(vertex);

			std::ifstream fragment("shaders/shadowFragment.frag.c");
			FragmentShader* fragmentShader = new FragmentShader(fragment);

			GeometryShader* geometryShader = 0;//new GeometryShader("");

			shaderProgramShadowPass = new ShaderProgramImpl(vertexShader, fragmentShader, geometryShader);
			shaderProgramShadowPass->link();
		}

		return shaderProgramShadowPass.get();
	}

	engine::ShaderProgram* getShaderProgramImageProcessing(const std::string& fragmentFile) {
		std::string filename = std::string("shaders/image-processing/") + fragmentFile;
		std::ifstream fragment(filename.c_str());
		FragmentShader* fragmentShader = new FragmentShader(fragment);

		engine::ShaderProgram* shaderProgramImageProcessing = new ShaderProgramImpl(0, fragmentShader, 0);
		shaderProgramImageProcessing->link();

		return shaderProgramImageProcessing;
	}

	void renderScene(std::vector<engine::Entity*>& actors, engine::Camera* camera, engine::ShaderProgram* shader, int drawFlags) {
		std::vector<engine::Entity*>::const_iterator ite;

		shader->bind();

		shader->setParameter("myProjectionMatrix", camera->getFrustum()->getProjectionMatrix());
		CHECK_ERROR();

		for(ite = actors.begin(); ite != actors.end(); ite++) {
			engine::Entity* entity = *ite;

			engine::math::Matrix4 modelViewMatrix = camera->getFrustum()->getViewMatrix() * entity->getTransformation();
			engine::math::Matrix3 normalMatrix = modelViewMatrix.normalMatrix();

			shader->setParameter("myModelViewMatrix", modelViewMatrix);
			CHECK_ERROR();

			shader->setParameter("myNormalMatrix", normalMatrix);
			CHECK_ERROR();

			renderEntity(entity, shader, drawFlags);
		}

		shader->unbind();
	}

	void geometryState(std::vector<engine::Entity*>& actors, engine::Camera* camera, engine::SceneManager* sceneManager) {

		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		glDisable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepth(1.0);
		glColorMask(true, true, true, true);
		glDepthMask(true);

		gbuffer->bind();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		depthPass(actors, camera, sceneManager);
		CHECK_ERROR();

		materialPass(actors, camera, sceneManager);
		CHECK_ERROR();

		gbuffer->unbind();

		glColorMask(true, true, true, true);
		glDepthMask(true);
	}

	void depthPass(std::vector<engine::Entity*>& entities, engine::Camera* camera, engine::SceneManager* sceneManager) {
		glColorMask(false, false, false, false);
		glDepthMask(true);

		renderScene(entities, camera, getShaderProgramDepthPass(), VERTEX);
	}

	void materialPass(std::vector<engine::Entity*>& entities, engine::Camera* camera, engine::SceneManager* sceneManager) {
		glColorMask(true, true, true, true);
		glDepthMask(false);

		renderScene(entities, camera, getShaderProgramMaterialPass(), VERTEX | NORMAL | TEXTURE | COLOR);
	}

	engine::math::Vector4 getScissorRect(const engine::math::Vector3& lightPosition, float lightRange, engine::Camera* myCamera,
	      int* viewport) {

		engine::math::Vector4 scissor;
		engine::math::Vector3 corners[2];

		//Get the camera world position
		engine::math::Vector3 diagonal;

		//If the camera distance to the light source is lesser than
		//the light range, return the description of the whole screen
		engine::math::Vector3 dir = myCamera->getFrustum()->getPosition() - lightPosition;
		if(dir.length() - lightRange <= 0.001) {
			return engine::math::Vector4(viewport[0], viewport[1], viewport[2], viewport[3]);
		}

		/**************************************/
		/*                    + Upper corner  */
		/*                   /                */
		/*                  /                 */
		/*                 /                  */
		/*                /                   */
		/*               /                    */
		/*              /                     */
		/*            ( ) Light position      */
		/*            /                       */
		/*           /                        */
		/*          /                         */
		/*         /                          */
		/*        /                           */
		/*       /                            */
		/*      + Lower corner                */
		/**************************************/

		//Multiply the light range by square root of two since we will compute
		//the corners of square
		lightRange *= 1.41421356f;

		const engine::math::Matrix4& modelviewMatrix = myCamera->getFrustum()->getViewMatrix();
		//const engine::math::Matrix4& projectionMatrix = myCamera->getProjectionMatrix();

		diagonal.x = modelviewMatrix.m00 + modelviewMatrix.m10;
		diagonal.y = modelviewMatrix.m01 + modelviewMatrix.m11;
		diagonal.z = modelviewMatrix.m02 + modelviewMatrix.m12;

		diagonal = diagonal * lightRange;

		//Compute the lower corner
		corners[0] = myCamera->project(lightPosition - diagonal, viewport);

		//Compute the upper corner
		corners[1] = myCamera->project(lightPosition + diagonal, viewport);

		//Set up the scissor info
		scissor.x = int(corners[0].x);
		scissor.y = int(corners[0].y);
		scissor.z = int(corners[1].x);
		scissor.w = int(corners[1].y);

		scissor.z -= scissor.x;
		scissor.w -= scissor.y;

		if(scissor.x < 0) {
			scissor.z += scissor.x;
			scissor.x = 0;
		}

		if(scissor.y < 0) {
			scissor.w += scissor.y;
			scissor.y = 0;
		}

		scissor.z = scissor.z > FRAMEBUFFER_SIZE ? FRAMEBUFFER_SIZE : scissor.z;
		scissor.w = scissor.w > FRAMEBUFFER_SIZE ? FRAMEBUFFER_SIZE : scissor.w;

		return scissor;
	}

	typedef std::map<engine::Light*, TextureImpl*> LightFramebuffer;
	LightFramebuffer lightFramebuffers;

	void genereteShadowInformation(std::vector<engine::Light*>& lights, engine::Camera* camera, engine::SceneManager* sceneManager) {


		std::vector<engine::Light*>::iterator ite;
		for(ite = lights.begin(); ite != lights.end(); ite++) {
			engine::Light* light = *ite;

			//bind framebuffer da luz
			LightFramebuffer::iterator iteFramebuffer = lightFramebuffers.find(light);
			if(iteFramebuffer == lightFramebuffers.end()) {
				TextureImpl* texture = new TextureRGBAFloatImpl(SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0);
				iteFramebuffer = lightFramebuffers.insert(std::make_pair(light, texture)).first;
			}

			shadowFramebuffer->setRenderTarget(iteFramebuffer->second, 0);

			shadowFramebuffer->bind();

//			glBlendFunc(GL_ONE, GL_ONE);
//			glEnable(GL_BLEND);

			glColorMask(true, true, true, true);

			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClearDepth(1.0);

			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);

			engine::ShaderProgram* shader = getShaderProgramShadowPass();
			shader->bind();

			engine::Frustum* lightCamera = light->getFrustum();
			std::vector<engine::Entity*> entities = sceneManager->getVisibleEntities(lightCamera);

			shader->setParameter("myLightPosition", lightCamera->getViewMatrix() * light->getPosition());

			shader->setParameter("myLightPosition2", light->getPosition());

			float zNear = 0.01f;
			float zFar = 200.0f;
			int numSplits = 1;

			float zAtual = zNear;
			float deltaZ = (zFar - zNear) / numSplits;

			for(int i = 0; i < numSplits; i++) {
				shader->setParameter("mySplit", i);
				shader->setParameter("myZNear", zAtual);
				shader->setParameter("myZFar", zAtual + deltaZ);
				lightCamera->setNear(zAtual);
				lightCamera->setFar(zAtual + deltaZ);

				std::vector<engine::Entity*>::iterator iteEntity;
				for(iteEntity = entities.begin(); iteEntity != entities.end(); iteEntity++) {
					engine::Entity* entity = *iteEntity;

					engine::math::Matrix4 modelViewMatrix = lightCamera->getViewMatrix() * entity->getTransformation();
					engine::math::Matrix3 normalMatrix = modelViewMatrix.normalMatrix();

					shader->setParameter("myProjectionMatrix", lightCamera->getProjectionMatrix());
					CHECK_ERROR();

					shader->setParameter("myModelViewMatrix", modelViewMatrix);
					CHECK_ERROR();

					shader->setParameter("myNormalMatrix", normalMatrix);
					CHECK_ERROR();

					renderEntity(entity, shader, VERTEX);
				}
			}

			shader->unbind();

			glColorMask(true, true, true, true);

//			glDisable(GL_BLEND);

			//unbind framebuffer da luz
			shadowFramebuffer->unbind();

//			glActiveTexture(GL_TEXTURE0);
//			glClientActiveTexture(GL_TEXTURE0);
//			framebuffer->getColorBuffer(0)->generateMipmap();

#define USE_FILTER 1
#if USE_FILTER
//			const char* filters[] = {
//					"laplacian.frag.c",
//					"sobel.frag.c",
//					"prewitt.frag.c",
//					"blur.frag.c",
//					"dilation.frag.c",
//					"erosion.frag.c",
//					"sharpen.frag.c",
//			};

#define NUM_FILTER 1

			const char* filters[][2] = {
					{"gaussian-y.frag.c", "gaussian-x.frag.c"},
					{"blur.frag.c", "pass.frag.c"},
			};

			if(blurShader == 0) {
				blurShader = getShaderProgramImageProcessing(filters[NUM_FILTER][0]);
			}

			if(passShader == 0) {
				passShader = getShaderProgramImageProcessing(filters[NUM_FILTER][1]);
			}

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, +1, 0, +1, -1, +1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_TEXTURE_2D);

			glDepthMask(false);

			filterFramebuffer->bind();
			applyFilter(blurShader.get(), shadowFramebuffer->getColorBuffer(0));
			filterFramebuffer->unbind();

			shadowFramebuffer->bind();
			applyFilter(passShader.get(), filterFramebuffer->getColorBuffer(0));
			shadowFramebuffer->unbind();

			glDepthMask(true);
			glDisable(GL_TEXTURE_2D);
#endif
		}
	}

	void applyFilter(engine::ShaderProgram* shader, engine::Texture* texture) {
		shader->bind();

		texture->bind(0);

		shader->setParameter("sampler0", 0);

		for(int x = 0; x < 3; x++) {
			for(int y = 0; y < 3; y++) {
				char tc_offset[32];

				sprintf(tc_offset, "tc_offset[%d]", y*3 + x);

				float tx = x / (float)(SHADOWMAP_SIZE >> 0);
				float ty = y / (float)(SHADOWMAP_SIZE >> 0);

				shader->setParameter(tc_offset, engine::math::Vector2(tx, ty));
			}
		}

		drawQuad();

		shader->unbind();

		texture->unbind(0);
	}

	void lightingState(std::vector<engine::Light*>& lights, engine::Camera* camera, engine::SceneManager* sceneManager) {

		genereteShadowInformation(lights, camera, sceneManager);

		lightbuffer->bind();

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(false);

		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_BLEND);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		engine::ShaderProgram* shader = getShaderProgramLigthingState();

		shader->bind();

		glEnable(GL_TEXTURE_2D);

		gtextures[COLOR0_BUFFER]->bind(0);
		shader->setParameter("myNormalMap", 0);
		CHECK_ERROR();

		gtextures[COLOR1_BUFFER]->bind(1);
		shader->setParameter("myDiffuseMap", 1);
		CHECK_ERROR();

		gtextures[COLOR2_BUFFER]->bind(2);
		shader->setParameter("mySpecularMap", 2);
		CHECK_ERROR();

		gtextures[DEPTH_BUFFER]->bind(3);
		shader->setParameter("myDepthMap", 3);
		CHECK_ERROR();

		//TODO Separar por tipo de Light

		/**
		 * m00, m01, m02, m03
		 * m10, m11, m12, m13
		 * m20, m21, m22, m23
		 * m30, m31, m32, m33
		 */
		engine::math::Matrix3 rotation = camera->getFrustum()->getViewMatrix().upperLeft();

		int viewport[4] = {0, 0, FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE};

		engine::math::Vector3 win[4] = {
			engine::math::Vector3(-1, -1, 0),
			engine::math::Vector3(-1, +1, 0),
			engine::math::Vector3(+1, +1, 0),
			engine::math::Vector3(+1, -1, 0),
		};

		engine::math::Vector2 tex[4] = {
			engine::math::Vector2(0, 0),
			engine::math::Vector2(0, 1),
			engine::math::Vector2(1, 1),
			engine::math::Vector2(1, 0),
		};

		engine::math::Vector3 obj[4] = {
				engine::math::Vector3(0,                0,                0),
				engine::math::Vector3(0,                FRAMEBUFFER_SIZE, 0),
				engine::math::Vector3(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE, 0),
				engine::math::Vector3(FRAMEBUFFER_SIZE, 0,                0),
		};

		for(int i = 0; i < 4; i++) {
			engine::math::Vector3 unprojected = camera->unproject(obj[i], viewport);
			obj[i] = rotation * (unprojected - camera->getFrustum()->getPosition()).normalize();
		}

		engine::math::Vector2 planes;
		float znear = 0.01;
		float zfar = 200;
		planes.x = - zfar / (zfar - znear);
		planes.y = - (zfar * znear) / (zfar - znear);
		shader->setParameter("myPlanes", planes);
		CHECK_ERROR();

		shader->setParameter("myCameraPosition", camera->getFrustum()->getPosition());
		CHECK_ERROR();

		shader->setParameter("myCameraDirection", camera->getFrustum()->getDirection());
		CHECK_ERROR();

		shader->setParameter("myVertex", win, 4);
		CHECK_ERROR();

		shader->setParameter("myTexCoord", tex, 4);
		CHECK_ERROR();

		shader->setParameter("myViewVertex", obj, 4);
		CHECK_ERROR();

		shader->setParameter("myViewMatrixInverse", camera->getFrustum()->getViewMatrix().inverse());
		CHECK_ERROR();

		glEnable(GL_SCISSOR_TEST);
		std::vector<engine::Light*>::iterator ite;
		for(ite = lights.begin(); ite != lights.end(); ite++) {
			engine::Light* light = *ite;

			engine::math::Vector4 scissor = getScissorRect(light->getPosition(), 90.0f, camera, viewport);

			if(scissor.z <= 1 || scissor.w <= 1) {
				continue;
			}

			glScissor((int)scissor.x, (int)scissor.y, (int)scissor.z, (int)scissor.w);
			CHECK_ERROR();

			LightFramebuffer::iterator framebufferIte = lightFramebuffers.find(light);
//			framebufferIte->second->getDepthBuffer()->bind(4);
			framebufferIte->second->bind(4);
			shader->setParameter("myShadowMap", 4);
			CHECK_ERROR();

			shader->setParameter("myLightColor", light->getDiffuse());
			CHECK_ERROR();

			shader->setParameter("myLightPosition", camera->getFrustum()->getViewMatrix() * light->getPosition());
			CHECK_ERROR();

			shader->setParameter("myLightPosition2", light->getPosition());
			CHECK_ERROR();

			shader->setParameter("myLightDirection", (camera->getFrustum()->getViewMatrix().normalMatrix() * light->getDirection()).normalize());
			CHECK_ERROR();

			shader->setParameter("myLightAttenuation", light->getAttenuations());
			CHECK_ERROR();

			shader->setParameter("myLightViewProjectionMatrix", light->getFrustum()->getViewProjectionMatrix());
			CHECK_ERROR();

			shader->setParameter("myLightViewMatrix", light->getFrustum()->getViewMatrix());
			CHECK_ERROR();

			shader->setParameter("myLightProjectionMatrix", light->getFrustum()->getProjectionMatrix());
			CHECK_ERROR();

			unsigned int index[] = {0, 1, 2, 3};
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, index);
			CHECK_ERROR();

//			framebufferIte->second->getDepthBuffer()->unbind(4);
			framebufferIte->second->unbind(4);
		}
		glDisable(GL_SCISSOR_TEST);

		gtextures[COLOR0_BUFFER]->unbind(0);
		CHECK_ERROR();

		gtextures[COLOR1_BUFFER]->unbind(1);
		CHECK_ERROR();

		gtextures[COLOR2_BUFFER]->unbind(2);
		CHECK_ERROR();

		gtextures[DEPTH_BUFFER]->unbind(3);
		CHECK_ERROR();

		shader->unbind();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(camera->getFrustum()->getProjectionMatrix().matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(camera->getFrustum()->getViewMatrix().matrix);

		engine::math::Vector3 corners[8];
		lights[1]->getFrustum()->getCorners(corners);

		glLineWidth(5.0);
		glColor3f(1, 1, 1);
		glBegin(GL_LINE_LOOP);
			glVertex3fv(corners[0].vector);
			glVertex3fv(corners[1].vector);
			glVertex3fv(corners[2].vector);
			glVertex3fv(corners[3].vector);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glVertex3fv(corners[4].vector);
			glVertex3fv(corners[5].vector);
			glVertex3fv(corners[6].vector);
			glVertex3fv(corners[7].vector);
		glEnd();

		glBegin(GL_LINES);
			glVertex3fv(corners[0].vector);
			glVertex3fv(corners[4].vector);
			glVertex3fv(corners[1].vector);
			glVertex3fv(corners[5].vector);
			glVertex3fv(corners[2].vector);
			glVertex3fv(corners[6].vector);
			glVertex3fv(corners[3].vector);
			glVertex3fv(corners[7].vector);
		glEnd();
		glLineWidth(1.0);

#if 0
		for(ite = lights.begin(); ite != lights.end(); ite++) {
			engine::Light* light = *ite;

			engine::math::Vector4 scissor = getScissorRect(light->getPosition(), 90.0f, camera, viewport);

			if(scissor.z <= 1 || scissor.w <= 1) {
				continue;
			}

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(projection.matrix);

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(modelview.matrix);

			scissor.x /= FRAMEBUFFER_SIZE;
			scissor.z /= FRAMEBUFFER_SIZE;

			scissor.y /= FRAMEBUFFER_SIZE;
			scissor.w /= FRAMEBUFFER_SIZE;

			glColor3fv(light->getDiffuse().vector);
			glBegin(GL_LINE_LOOP);
				glVertex2f(scissor.x,           scissor.y);
				glVertex2f(scissor.x,           scissor.y+scissor.w);
				glVertex2f(scissor.x+scissor.z, scissor.y+scissor.w);
				glVertex2f(scissor.x+scissor.z, scissor.y);
			glEnd();

			glColor3f(1, 0, 0);
			glPointSize(5);
			glBegin(GL_POINTS);
				engine::math::Vector3 position = camera->project(light->getPosition(), viewport);
				glVertex2f(position.x / FRAMEBUFFER_SIZE, position.y / FRAMEBUFFER_SIZE);
			glEnd();
		}
#endif

		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);
//		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		lightbuffer->unbind();
	}

	void postProcessingState(engine::Camera* camera, engine::SceneManager* sceneManager) {
	}

	void finalStage(engine::Camera* camera, engine::SceneManager* sceneManager) {
	}

	static const int FRAMEBUFFER_SIZE = 512;
	static const int SHADOWMAP_SIZE = 256;


	void createFramebuffer() {
		gbuffer = new FramebufferImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE);
		gtextures[COLOR0_BUFFER] = new TextureRGBAFloatImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE, 0);
		gbuffer->setRenderTarget(gtextures[COLOR0_BUFFER], COLOR0_BUFFER);

		gtextures[COLOR1_BUFFER] = new TextureRGBAFloatImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE, 0);
		gbuffer->setRenderTarget(gtextures[COLOR1_BUFFER], COLOR1_BUFFER);

		gtextures[COLOR2_BUFFER] = new TextureRGBAFloatImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE, 0);
		gbuffer->setRenderTarget(gtextures[COLOR2_BUFFER], COLOR2_BUFFER);

		gtextures[DEPTH_BUFFER] = new DepthTextureImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE);
		gbuffer->setDepthTarget(gtextures[DEPTH_BUFFER]);

//		if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
//			throw engine::Exception("Framebuffer incompleto");
//		}

		lightbuffer = new FramebufferImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE);
		lighttexture = new TextureRGBAFloatImpl(FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE, 0);
		lightbuffer->setRenderTarget(lighttexture, 0);

		filterFramebuffer = new FramebufferImpl(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		filtertexture = new TextureRGBAFloatImpl(SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0);
		filterFramebuffer->setRenderTarget(filtertexture, 0);

		shadowFramebuffer = new FramebufferImpl(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	}

	void drawQuad() {
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);

			glTexCoord2f(0, 1);
			glVertex2f(0, 1);

			glTexCoord2f(1, 1);
			glVertex2f(1, 1);

			glTexCoord2f(1, 0);
			glVertex2f(1, 0);
		glEnd();
	}
public:
	DeferredShading(int _width, int _height) :
		width(_width), height(_height), frameBufferCreated(false),
		shaderProgramDepthPass(0), shaderProgramMaterialPass(0), shaderProgramLightingPass(0),
		shaderProgramShadowPass(0), blurShader(0), passShader(0) {
	}

	virtual ~DeferredShading() {
	}

	virtual void render(engine::Camera* camera, engine::SceneManager* sceneManager) {
		if(!frameBufferCreated) {
			frameBufferCreated = true;
			createFramebuffer();
		}

		//TODO Culling
		std::vector<engine::Entity*> entities = sceneManager->getVisibleEntities(camera->getFrustum());
		std::vector<engine::Light*> ligths = sceneManager->getVisibleLights(camera->getFrustum());

		geometryState(entities, camera, sceneManager);
		lightingState(ligths, camera, sceneManager);
		postProcessingState(camera, sceneManager);
		finalStage(camera, sceneManager);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, +1, 0, +1, -1, +1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);

#if 1
		glColor4f(1, 1, 1, 1);

		lighttexture->bind(0);
//		lightFramebuffers.begin()->second->getDepthBuffer()->bind(0);
//		lightFramebuffers.begin()->second->getColorBuffer(0)->bind(0);
//		glGenerateMipmapEXT(GL_TEXTURE_2D);

		drawQuad();

		lighttexture->unbind(0);
//		lightFramebuffers.begin()->second->getDepthBuffer()->unbind(0);
//		lightFramebuffers.begin()->second->getColorBuffer(0)->unbind(0);

#else
		gtextures[COLOR0_BUFFER]->bind(0);
		glPushMatrix();
			glTranslatef(0, 0, 0);
			glScalef(0.5, 0.5, 1);
			drawQuad();
		glPopMatrix();
		gtextures[COLOR0_BUFFER]->unbind(0);

		gtextures[COLOR1_BUFFER]->bind(0);
		glPushMatrix();
			glTranslatef(0.5, 0, 0);
			glScalef(0.5, 0.5, 1);
			drawQuad();
		glPopMatrix();
		gtextures[COLOR1_BUFFER]->unbind(0);

		gtextures[COLOR2_BUFFER]->bind(0);
		glPushMatrix();
			glTranslatef(0.5, 0.5, 0);
			glScalef(0.5, 0.5, 1);
			drawQuad();
		glPopMatrix();
		gtextures[COLOR2_BUFFER]->unbind(0);

		gtextures[DEPTH_BUFFER]->bind(0);
		glPushMatrix();
			glTranslatef(0, 0.5, 0);
			glScalef(0.5, 0.5, 1);
			drawQuad();
		glPopMatrix();
		gtextures[DEPTH_BUFFER]->unbind(0);
#endif

		CHECK_ERROR();

		glDisable(GL_TEXTURE_2D);
	}

	virtual void render(engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
		//TODO implementar
	}

	virtual void resize(int _width, int _height) {
		width = _width;
		height = _height;

		glViewport(0, 0, width, height);
	}

	virtual int getWidth() {
		return width;
	}

	virtual int getHeight() {
		return height;
	}
};

#endif /* DEFERREDSHADINGRENDERER_H_ */
