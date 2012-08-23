#ifndef LIGHTPREPASS_H_
#define LIGHTPREPASS_H_

#include "Shader.h"
#include "MyImpls.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "RenderEntity.h"
#include "Geometry.h"
#include "GameEntity.h"

#include "SparseVoxelOctreeBuilder.h"
#include "SparseVoxelOctree.h"
#include "Model.h"

#include <fstream>
#include <iostream>


const int GEOMETRY_FRAMEBUFFER_SIZE = 512;
const int LIGHT_FRAMEBUFFER_SIZE = 512;
const int SHADOWMAP_SIZE = 256;

class DeferredLighting : public engine::Renderer {
	bool frameBufferCreated;

	engine::Framebuffer* gbuffer;
	engine::Texture* depthTexture;
	engine::Texture* normalTexture;
	engine::Texture* objectIdTexture;

	engine::Framebuffer* lbuffer;
	engine::Texture* lightTexture;

	engine::Framebuffer* filterFramebuffer;
	engine::Texture* filterTexture;

	engine::Shader* shaderProgramLightPass;
	engine::Shader* shaderProgramMaterialPass;

	engine::Shader* blurShader;
	engine::Shader* passShader;

	typedef std::map<const engine::Light*, engine::Texture*> LightFramebuffer;
	LightFramebuffer lightFramebuffers;
	engine::Framebuffer* shadowFramebuffer;
	engine::Texture* depthTextureShadow;

	engine::Shader* getShaderProgramLightPass() {
		if(!shaderProgramLightPass) {
			std::ifstream vertex("shaders/light-pre-pass/lightVertex.vert.c");
			engine::Source* vertexShader = new engine::Source(vertex);

			std::ifstream fragment("shaders/light-pre-pass/lightFragment.frag.c");
			engine::Source* fragmentShader = new engine::Source(fragment);

			shaderProgramLightPass = new engine::Shader(vertexShader, fragmentShader, 0);
			shaderProgramLightPass->link();
		}

		return shaderProgramLightPass;
	}

	engine::Shader* getShaderProgramMaterialPass() {
		if(!shaderProgramMaterialPass) {
			std::ifstream vertex("shaders/light-pre-pass/materialVertex.vert.c");
			engine::Source* vertexShader = new engine::Source(vertex);

			std::ifstream fragment("shaders/light-pre-pass/materialFragment.frag.c");
			engine::Source* fragmentShader = new engine::Source(fragment);

			shaderProgramMaterialPass = new engine::Shader(vertexShader, fragmentShader, 0);
			shaderProgramMaterialPass->link();
		}

		return shaderProgramMaterialPass;
	}

	engine::Shader* getShaderProgramImageProcessing(const std::string& fragmentFile) {
		std::string filename = std::string("shaders/image-processing/") + fragmentFile;

		std::ifstream fragment(filename.c_str());
		engine::Source* fragmentShader = new engine::Source(fragment);

		std::ifstream vertex("shaders/image-processing/vertex.vert.c");
		engine::Source* vertexShader = new engine::Source(vertex);

		engine::Shader* shaderProgramImageProcessing = new engine::Shader(vertexShader, fragmentShader, 0);
		shaderProgramImageProcessing->link();

		return shaderProgramImageProcessing;
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

	void createFramebuffer() {
		normalTexture = 0;//new engine::Texture(engine::TextureFloatFormat(engine::TextureFormat::RGBA, 16), GEOMETRY_FRAMEBUFFER_SIZE, GEOMETRY_FRAMEBUFFER_SIZE);// gbuffer->createColorBuffer((0 << 16) | 0);
		objectIdTexture = 0;//new engine::Texture(engine::TextureFloatFormat(engine::TextureFormat::RGBA, 16), GEOMETRY_FRAMEBUFFER_SIZE, GEOMETRY_FRAMEBUFFER_SIZE);//gbuffer->createColorBuffer((0 << 16) | 1);
		depthTexture = 0;//new engine::Texture(engine::TextureUnsignedNormalizedFormat(engine::TextureFormat::DEPTH, 24), GEOMETRY_FRAMEBUFFER_SIZE, GEOMETRY_FRAMEBUFFER_SIZE);

		gbuffer = new engine::Framebuffer(GEOMETRY_FRAMEBUFFER_SIZE, GEOMETRY_FRAMEBUFFER_SIZE);
		gbuffer->setRenderTarget(normalTexture, 0);
		gbuffer->setRenderTarget(objectIdTexture, 1);
		gbuffer->setDepthTarget(depthTexture);

		lightTexture = 0;//new engine::Texture(engine::TextureFloatFormat(engine::TextureFormat::RGBA, 16), LIGHT_FRAMEBUFFER_SIZE, LIGHT_FRAMEBUFFER_SIZE); //lbuffer->createColorBuffer(0);
		lbuffer = new engine::Framebuffer(LIGHT_FRAMEBUFFER_SIZE, LIGHT_FRAMEBUFFER_SIZE);
		lbuffer->setRenderTarget(lightTexture, 0);

		filterTexture = 0;//new engine::Texture(engine::TextureUnsignedNormalizedFormat(engine::TextureFormat::RGBA, 16), SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		filterFramebuffer = new engine::Framebuffer(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		filterFramebuffer->setRenderTarget(filterTexture, 0);

		shadowFramebuffer = new engine::Framebuffer(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		depthTextureShadow = 0;//new engine::Texture(engine::TextureUnsignedNormalizedFormat(engine::TextureFormat::DEPTH, 24), SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		shadowFramebuffer->setDepthTarget(depthTextureShadow);
	}

	void renderScene(std::vector<engine::ModelInstance*>& geometries, const engine::Frustum* frustum, engine::GraphicManager* graphicManager, const std::string& aspect) {
		engine::RenderEntity renderEntity(aspect);
		renderEntity.renderScene(geometries, frustum, graphicManager, 0);
	}

	/**
	 * Render to depth and normal textures
	 */
	void geometryPass(std::vector<engine::ModelInstance*>& geometries, const engine::Frustum* camera, engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
		graphicManager->bindFramebuffer(gbuffer);

		graphicManager->cullFace(GL_BACK, GL_CCW);
		graphicManager->disableTexture();

		graphicManager->depthTest(GL_LEQUAL);
		graphicManager->clear(engine::math::Vector4(0.0, 0.0, 0.0, 1.0), 1.0);

		//early Z
		graphicManager->disableColorWrite();
		renderScene(geometries, camera, graphicManager, "GEOMETRY-EARLY-Z");

		graphicManager->disableDepthWrite();
		graphicManager->enableColorWrite();
		renderScene(geometries, camera, graphicManager, "GEOMETRY");

		graphicManager->enableDepthWrite();

		graphicManager->unbindShader();
		graphicManager->bindFramebuffer(engine::Framebuffer::DEFAULT);
	}

	/**
	 * Do foward rendering
	 */
	void materialPass(std::vector<engine::ModelInstance*>& geometries, const engine::Frustum* camera, engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {

//		applyFilter(lbuffer.get(), 0);

		graphicManager->depthTest(GL_LEQUAL);
		graphicManager->clear(engine::math::Vector4(0.0, 0.0, 0.0, 1.0), 1.0);

		engine::Shader* const shader = getShaderProgramMaterialPass();

		graphicManager->setShader(shader);

		engine::Constant* myBufferSize = shader->getConstant("myBufferSize");
		engine::Constant* myScreenSize = shader->getConstant("myScreenSize");

		myBufferSize->setValue(engine::math::Vector2(LIGHT_FRAMEBUFFER_SIZE, LIGHT_FRAMEBUFFER_SIZE));
		myScreenSize->setValue(engine::math::Vector2(graphicManager->getWidth(), graphicManager->getHeight()));

		graphicManager->enableTexture();

		//TODO como passar essas texturas para o shader??
		graphicManager->addSampler("lightBuffer", lightTexture);
		graphicManager->addSampler("objectIdBuffer", objectIdTexture);

		renderScene(geometries, camera, graphicManager, "MATERIAL");

		graphicManager->disableTexture();
		graphicManager->removeSamplers();
	}

	void globalIlumination(const engine::Frustum* camera, engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
//		drawQuad(graphicManager, );
	}

	/**
	 * Render to light buffer and acumulate the lights contribuition
	 */
	void lightPass(std::vector<engine::ModelInstance*>& geometries, const engine::Frustum* camera, engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
		std::vector<engine::Light*> lights = sceneManager->getVisibleLights(camera);

		gpuPrint("shadow begin");
		genereteShadowInformation(lights, camera, graphicManager, sceneManager);
		gpuPrint("shadow end");

		graphicManager->bindFramebuffer(lbuffer);

		graphicManager->enableTexture();
		graphicManager->enableAditiveBlend();
		graphicManager->disableDepthWrite();

		graphicManager->clear(engine::math::Vector4(0.0, 0.0, 0.0, 0.0));

		globalIlumination(camera, graphicManager, sceneManager);

		engine::Shader* const shader = getShaderProgramLightPass();

		graphicManager->setShader(shader);
		engine::Constant* myZNear = shader->getConstant("myZNear");
		engine::Constant* myZFar = shader->getConstant("myZFar");
		engine::Constant* myPlanes = shader->getConstant("myPlanes");
		engine::Constant* myCameraPosition = shader->getConstant("myCameraPosition");
		engine::Constant* myProjectionMatrix = shader->getConstant("myProjectionMatrix");
		engine::Constant* myViewMatrixInverse = shader->getConstant("myViewMatrixInverse");
		engine::Constant* myViewMatrix = shader->getConstant("myViewMatrix");
		engine::Constant* myLightAttenuations = shader->getConstant("myLightAttenuations");
		engine::Constant* castShadow = shader->getConstant("castShadow");
		engine::Constant* myShadowMap = shader->getConstant("myShadowMap");
		engine::Constant* myLightPositionInCameraSpace = shader->getConstant("myLightPositionInCameraSpace");
		engine::Constant* myLightDirectionInCameraSpace = shader->getConstant("myLightDirectionInCameraSpace");
		engine::Constant* myLightColor = shader->getConstant("myLightColor");
		engine::Constant* myLightViewMatrix = shader->getConstant("myLightViewMatrix");
		engine::Constant* myLightProjectionMatrix = shader->getConstant("myLightProjectionMatrix");

		engine::Attribute* myVertex = shader->getAttribute("myVertex");
		engine::Attribute* myTexCoord = shader->getAttribute("myTexCoord");
		engine::Attribute* myViewVertex = shader->getAttribute("myViewVertex");

		myZNear->setValue(camera->getNear());
		myZFar->setValue(camera->getFar());

		engine::math::Vector2 planes;
		float znear = camera->getNear();
		float zfar = camera->getFar();
		planes.x = - zfar / (zfar - znear);
		planes.y = - (zfar * znear) / (zfar - znear);
		myPlanes->setValue(planes);

		myCameraPosition->setValue(camera->getPosition());
		CHECK_ERROR();

		const int viewport[4] = {0, 0, LIGHT_FRAMEBUFFER_SIZE, LIGHT_FRAMEBUFFER_SIZE};

		const engine::math::Vector3 win[4] = {
			engine::math::Vector3(0, 0, 0),
			engine::math::Vector3(0, 1, 0),
			engine::math::Vector3(1, 1, 0),
			engine::math::Vector3(1, 0, 0),
		};

		const engine::math::Vector2 tex[4] = {
			engine::math::Vector2(0, 0),
			engine::math::Vector2(0, 1),
			engine::math::Vector2(1, 1),
			engine::math::Vector2(1, 0),
		};

		engine::math::Vector3 obj[4] = {
				engine::math::Vector3(0,                      0,                      0),
				engine::math::Vector3(0,                      LIGHT_FRAMEBUFFER_SIZE, 0),
				engine::math::Vector3(LIGHT_FRAMEBUFFER_SIZE, LIGHT_FRAMEBUFFER_SIZE, 0),
				engine::math::Vector3(LIGHT_FRAMEBUFFER_SIZE, 0,                      0),
		};

		const engine::math::Matrix3 rotation = camera->getViewMatrix().upperLeft();
		for(int i = 0; i < 4; i++) {
			const engine::math::Vector3 unprojected = camera->unproject(obj[i], viewport);
			obj[i] = rotation * (unprojected - camera->getPosition()).normalize();
		}

		myVertex->setValue(win, 4);
		myTexCoord->setValue(tex, 4);
		myViewVertex->setValue(obj, 4);

		const engine::math::Matrix4 projection = engine::math::Matrix4::orthoMatrix(0, 1, 0, 1, -1, 1);
		myProjectionMatrix->setValue(projection);
		myViewMatrixInverse->setValue(camera->getViewMatrix().inverse());
		myViewMatrix->setValue(camera->getViewMatrix());

//		shader->bindTextureUnit(0, "myDepthMap", depthTexture);
//		shader->bindTextureUnit(1, "myNormalMap", normalTexture);

		std::vector<engine::Light*>::const_iterator ite;
		for(ite = lights.begin(); ite != lights.end(); ite++) {
			const engine::Light* const light = *ite;

			const engine::math::Vector3 attenuations = light->getAttenuations();
			const engine::math::Vector4 scissor = light->scissorRect(attenuations.x, camera, viewport);

//			if(scissor.z <= 1 || scissor.w <= 1) {
//				continue;
//			}
//
//			graphicManager->scissorTest(scissor);

			myLightAttenuations->setValue(light->getAttenuations());
			castShadow->setValue(light->castShadow());

			LightFramebuffer::iterator framebufferIte = lightFramebuffers.find(light);
			if(light->castShadow()) {
				//shader->bindTextureUnit(2, "myShadowMap", framebufferIte->second);
			}

			myLightPositionInCameraSpace->setValue(camera->getViewMatrix() * light->getPosition());
			myLightDirectionInCameraSpace->setValue((camera->getViewMatrix().normalMatrix() * light->getDirection()).normalize());
			myLightColor->setValue(light->getDiffuse());
			myLightViewMatrix->setValue(light->getFrustum()->getViewMatrix());
			myLightProjectionMatrix->setValue(light->getFrustum()->getProjectionMatrix());

			unsigned short index[] = {0, 1, 2, 0, 2, 3};
			graphicManager->drawIndex(index, sizeof(index) / sizeof(unsigned short));

			if(light->castShadow()) {
				//shader->unbindTextureUnit(2);
			}
		}

		graphicManager->disableScissor();

		//shader->unbindTextures();

		graphicManager->unbindShader();

		graphicManager->disableTexture();
		graphicManager->disableBlend();
		graphicManager->disableDepthWrite();
//		graphicManager->enableColorWrite();

		graphicManager->bindFramebuffer(engine::Framebuffer::DEFAULT);
	}

	void genereteShadowInformation(std::vector<engine::Light*>& lights, const engine::Frustum* camera, engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
		std::vector<engine::Light*>::const_iterator ite;
		for(ite = lights.begin(); ite != lights.end(); ite++) {
			const engine::Light* const light = *ite;

			if(light->castShadow()) {
				LightFramebuffer::iterator iteFramebuffer = lightFramebuffers.find(light);
				if(iteFramebuffer == lightFramebuffers.end()) {
					engine::Texture* texture = 0;//new engine::Texture(engine::TextureUnsignedNormalizedFormat(engine::TextureFormat::RGBA, 16), SHADOWMAP_SIZE, SHADOWMAP_SIZE);

					iteFramebuffer = lightFramebuffers.insert(std::make_pair(light, texture)).first;
				}

				shadowFramebuffer->setRenderTarget(iteFramebuffer->second, 0);
				graphicManager->bindFramebuffer(shadowFramebuffer);

//				graphicManager->enableColorWrite();
				graphicManager->depthTest(GL_LEQUAL);
				graphicManager->clear(engine::math::Vector4(1.0, 1.0, 1.0, 1.0), 1.0);

				const engine::Frustum* const frustum = light->getFrustum();
				std::vector<engine::ModelInstance*> geometries = sceneManager->getVisibleGeometries(frustum);

				renderScene(geometries, frustum, graphicManager, "SHADOW");

				graphicManager->unbindShader();
//				graphicManager->enableColorWrite();
				graphicManager->bindFramebuffer(engine::Framebuffer::DEFAULT);

				applyFilter(graphicManager, shadowFramebuffer, 0);
			}
		}
	}

	void applyFilter(engine::GraphicManager* graphicManager, engine::Framebuffer* framebuffer, int target) {
		const int FILTER = 1;

		const char* filters[][2] = {
				{"gaussian-y.frag.c", "gaussian-x.frag.c"},
				{"blur.frag.c", "pass.frag.c"},
		};

		if(blurShader == 0) {
			blurShader = getShaderProgramImageProcessing(filters[FILTER][0]);
		}

		if(passShader == 0) {
			passShader = getShaderProgramImageProcessing(filters[FILTER][1]);
		}

		graphicManager->disableDepthTest();
		graphicManager->disableCullFace();
		graphicManager->enableTexture();

		graphicManager->disableDepthWrite();

		graphicManager->bindFramebuffer(filterFramebuffer);
		applyFilter(graphicManager, blurShader, framebuffer->getColorBuffer(target));
		graphicManager->bindFramebuffer(engine::Framebuffer::DEFAULT);

		graphicManager->bindFramebuffer(framebuffer);
		applyFilter(graphicManager, passShader, filterFramebuffer->getColorBuffer(0));
		graphicManager->bindFramebuffer(engine::Framebuffer::DEFAULT);

		graphicManager->enableDepthWrite();
		graphicManager->disableTexture();
	}

	void applyFilter(engine::GraphicManager* graphicManager, engine::Shader* shader, engine::Texture* texture) {
		engine::math::Matrix4 ortho = engine::math::Matrix4::orthoMatrix(0, 1, 0, 1);

		shader->bind();

		shader->getConstant("myProjectionMatrix")->setValue(ortho);
		//shader->bindTextureUnit(0, "sampler0", texture);

		engine::Constant* tc_offset[9];

		for(int i = 0; i < 9; i++) {
			char name[32];

			sprintf(name, "tc_offset[%d]", i);
			tc_offset[i] = shader->getConstant(name);
		}

		for(int x = 0; x < 3; x++) {
			for(int y = 0; y < 3; y++) {
				int index = y*3 + x;

				float tx = x / (float)(SHADOWMAP_SIZE >> 0);
				float ty = y / (float)(SHADOWMAP_SIZE >> 0);

				tc_offset[index]->setValue(engine::math::Vector2(tx, ty));
			}
		}

		engine::Attribute* myVertex = shader->getAttribute("myVertex");
		engine::Attribute* myTexCoord = shader->getAttribute("myTexCoord");

		drawQuad(graphicManager, myVertex, myTexCoord);

		//shader->unbindTextures();
		shader->unbind();
	}

public:
	DeferredLighting() :
		frameBufferCreated(false),
		gbuffer(0),
		depthTexture(0),
		normalTexture(0),
		objectIdTexture(0),
		lbuffer(0),
		lightTexture(0),
		filterFramebuffer(0),
		filterTexture(0),
		shaderProgramLightPass(0),
		shaderProgramMaterialPass(0),
		blurShader(0),
		passShader(0),
		shadowFramebuffer(0),
		depthTextureShadow(0) { }

	virtual ~DeferredLighting() {
		delete gbuffer;
		delete depthTexture;
		delete normalTexture;
		delete objectIdTexture;

		delete lbuffer;
		delete lightTexture;

		delete filterFramebuffer;
		delete filterTexture;

		delete shaderProgramLightPass;
		delete shaderProgramMaterialPass;

		delete blurShader;
		delete passShader;

		for(LightFramebuffer::iterator ite = lightFramebuffers.begin(); ite != lightFramebuffers.end(); ite++) {
			delete ite->second;
		}

		delete shadowFramebuffer;
		delete depthTextureShadow;
	}

	virtual void render(engine::GraphicManager* graphicManager, engine::SceneManager* sceneManager) {
		if(!frameBufferCreated) {
			frameBufferCreated = true;
			createFramebuffer();
		}

		const engine::Frustum* const camera = sceneManager->getActualCamera()->getComponent<engine::Frustum>();
		std::vector<engine::ModelInstance*> geometries = sceneManager->getVisibleGeometries(camera);

		std::cout << "entidades visiveis: " << geometries.size() << std::endl;

		gpuPrint("geometry begin");
		geometryPass(geometries, camera, graphicManager, sceneManager);
		gpuPrint("geometry end");

		gpuPrint("light begin");
		lightPass(geometries, camera, graphicManager, sceneManager);
		gpuPrint("light end");

		gpuPrint("material begin");
		materialPass(geometries, camera, graphicManager, sceneManager);
		gpuPrint("material end");

//		if(GLEW_GREMEDY_string_marker) {
//			char str[] = "begin: normalTexture";
//			glStringMarkerGREMEDY(sizeof(str), str);
//		}
//
//		drawTexture(graphicManager, normalTexture);
//
//		if(GLEW_GREMEDY_string_marker) {
//			char str[] = "end: normalTexture";
//			glStringMarkerGREMEDY(sizeof(str), str);
//		}

//		drawTexture(graphicManager, this->normalTexture);
	}

	void gpuPrint(const char* str) {
		if(GLEW_GREMEDY_string_marker) {
			glStringMarkerGREMEDY(0, str);
		}
	}

	void drawTexture(engine::GraphicManager* graphicManager, engine::Texture* texture) {
		graphicManager->disableDepthTest();
		graphicManager->disableCullFace();
		graphicManager->enableTexture();

		graphicManager->disableDepthWrite();

		static engine::Shader* shader = 0;

		if(!shader) {
			shader = getShaderProgramImageProcessing("pass.frag.c");
		}

		shader->bind();
		//shader->bindTextureUnit(0, "sampler0", texture);
		shader->getConstant("myProjectionMatrix")->setValue(engine::math::Matrix4::orthoMatrix(0, 1, 0, 1));
		drawQuad(graphicManager, shader->getAttribute("myVertex"), shader->getAttribute("myTexCoord"));
		shader->unbind();

		graphicManager->enableDepthWrite();
		graphicManager->disableTexture();
	}
};

#endif /* LIGHTPREPASS_H_ */
