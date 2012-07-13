/*
 * SoftwareRenderer.h
 *
 *  Created on: 18/01/2011
 *      Author: marrony.neris
 */

#ifndef SOFTWARERENDERER_H_
#define SOFTWARERENDERER_H_

#include "Engine.h"
#include "GL/ShaderImpl.h"
#include "MyImpls.h"
#include "GL/TextureImpl.h"
#include "GL/FramebufferImpl.h"

#include "Rasterizer.h"
#include "Rasterizer2.h"

class VertexProgramImpl : public VertexProgram {
	mat4 projectionMatrix;
	mat4 viewMatrix;
public:
	virtual void process(VertexData& data) {
		vec4 myVertex = data.attrib("myVertex");

		mat4 modelViewProjection = projectionMatrix * viewMatrix;
		mat3 normalMatrix = viewMatrix.normalMatrix();

		vec4 temp = data.attrib("myNormal");
		vec3 normal = normalMatrix * vec3(temp.x, temp.y, temp.z);

		vec4 proj = modelViewProjection * myVertex;
		vec4 pos = viewMatrix * myVertex;

		data.varying(0, proj);
		data.varying(1, pos);
		data.varying(2, vec4(normal.x, normal.y, normal.z, 0));
		data.varying(3, data.attrib("myColor"));

//		data.varying(2, data.attrib("myTexCoord0"));
//		data.varying(3, data.attrib("myTexCoord1"));
	}

	void setProjectionMatrix(const engine::math::Matrix4& _projectionMatrix) {
		projectionMatrix = _projectionMatrix;
	}

	void setViewMatrix(const engine::math::Matrix4& _viewMatrix) {
		viewMatrix = _viewMatrix;
	}
};

class FragmentProgramImpl : public FragmentProgram {
	vec3 lightPosition;
	vec3 lightColor;
public:
	virtual void process(FragmentData& data) {
		vec4 temp = data.varying(1);

		vec3 position = vec3(temp.x, temp.y, temp.z);

		vec3 viewDir = (-position).normalize();

		vec3 lightDir = lightPosition - position;
//		vec3 halfVector = (viewDir + lightDir).normalize();

		lightDir = lightDir.normalize();

		temp = data.varying(2);
		vec3 normal = vec3(temp.x, temp.y, temp.z).normalize();

		vec3 Idiff = std::max(engine::math::dot(lightDir, normal), 0.0f) * lightColor;

		data.fragColor(vec4(Idiff.x, Idiff.y, Idiff.z, 1));
	}

	void setLightPosition(const engine::math::Vector3& p) {
		lightPosition = vec3(p.x, p.y, p.z);
	}

	void setLightColor(const engine::math::Vector3& p) {
		lightColor = vec3(p.x, p.y, p.z);
	}
};


class SoftwareRenderer : implements engine::Renderer {
	int width, height;

	void drawQuad(float x, float y, float width, float height) {
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(x, y);

			glTexCoord2f(0, 1);
			glVertex2f(x, y+height);

			glTexCoord2f(1, 1);
			glVertex2f(x+width, y+height);

			glTexCoord2f(1, 0);
			glVertex2f(x+width, y);
		glEnd();
	}

	void drawQuad() {
		drawQuad(0, 0, 1, 1);
	}

	void drawTexture(engine::Texture* texture) {
		drawTexture(texture, 0, 0, 1, 1);
	}

	void enter2D(float x, float y, float width, float height) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(x, width, y, height, -1, +1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void leave2D() {
	}

	void drawTexture(engine::Texture* texture, float x, float y, float width, float height) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		enter2D(x, y, width, height);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);

		glColor4f(1, 1, 1, 1);

		texture->bind(0);
		drawQuad(x, y, width, height);
		texture->unbind(0);

		CHECK_ERROR();

		glDisable(GL_TEXTURE_2D);

		leave2D();
	}

	void renderScene(engine::Camera* camera, engine::SceneManager* sceneManager, Rasterizer2* raster, VertexProgramImpl* vertProgram, FragmentProgramImpl* fragProgram) {
		std::vector<engine::Entity*> entities = sceneManager->getVisibleEntities(camera->getFrustum());
		std::vector<engine::Light*> lights = sceneManager->getVisibleLights(camera->getFrustum());

		vertProgram->setProjectionMatrix(camera->getFrustum()->getProjectionMatrix());

		std::vector<engine::Entity*>::iterator iteEntity;
		for(iteEntity = entities.begin(); iteEntity != entities.end(); iteEntity++) {
			engine::Entity* entity = *iteEntity;

			vertProgram->setViewMatrix(camera->getFrustum()->getViewMatrix() * entity->getTransformation());
			renderEntity(entity, camera, lights.front(), raster, vertProgram, fragProgram);
		}
	}

	void renderEntity(engine::Entity* actor, engine::Camera* camera, engine::Light* light, Rasterizer2* raster, VertexProgramImpl* vertProgram, FragmentProgramImpl* fragProgram) {
		engine::mesh::Mesh* mesh = actor->getMesh();

		fragProgram->setLightPosition(camera->getFrustum()->getViewMatrix() * light->getPosition());
		fragProgram->setLightColor(light->getDiffuse());

		engine::Buffer *vertexBuffer = mesh->getVertexBuffer();
		if(vertexBuffer->isOk()) {
			raster->vertexAttrib("myVertex", vertexBuffer->getMode(), vertexBuffer->getStride(), vertexBuffer->getPointer());
		}

		engine::Buffer *normalBuffer = mesh->getNormalBuffer();
		if(normalBuffer->isOk()) {
			raster->vertexAttrib("myNormal", normalBuffer->getMode(), normalBuffer->getStride(), normalBuffer->getPointer());
		}

		engine::Buffer *colorBuffer = mesh->getColorBuffer();
		if(colorBuffer->isOk()) {
			raster->vertexAttrib("myColor", colorBuffer->getMode(), colorBuffer->getStride(), colorBuffer->getPointer());
		}

		engine::Material* material = actor->getMaterial();
		for(int index = 0; index < material->getTextureCount(); index++) {
			engine::Buffer *textureBuffer = mesh->getTextureBuffer();

			if(textureBuffer->isOk()) {
				char buffer[256];

				sprintf(buffer, "myTexCoord%d", index);
				raster->vertexAttrib(buffer, textureBuffer->getMode(), textureBuffer->getStride(), textureBuffer->getPointer());
			}
		}

		engine::Buffer *indexBuffer = mesh->getIndexBuffer();
		if(indexBuffer->isOk()) {
			raster->drawTriangles((unsigned int*)indexBuffer->getPointer(), indexBuffer->getCount());
		}
	}
public:
	SoftwareRenderer(int _width, int _height) :
		width(_width), height(_height) {
	}

	virtual ~SoftwareRenderer() {
	}

	virtual void render(engine::Camera* camera, engine::SceneManager* sceneManager) {
		const int WINDOW_WIDTH  = width;
		const int WINDOW_HEIGHT = height;

		VertexProgramImpl vertProgram;
		FragmentProgramImpl fragProgram;

		Rasterizer2 rast(&vertProgram, &fragProgram);

		uint32_t* pixels = new uint32_t[WINDOW_WIDTH*WINDOW_HEIGHT];
		rast.setFrameBuffer(pixels, WINDOW_WIDTH, WINDOW_HEIGHT);
		rast.clear();

		renderScene(camera, sceneManager, &rast, &vertProgram, &fragProgram);

		unsigned char* ptr_pixels = (unsigned char*)pixels;

		engine::Texture* tex = new TextureRGBAImpl(WINDOW_WIDTH, WINDOW_HEIGHT, (unsigned char*)ptr_pixels);

		drawTexture(tex);

		delete tex;
		delete[] pixels;
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


#endif /* SOFTWARERENDERER_H_ */
