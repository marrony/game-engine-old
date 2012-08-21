/*
 * Scene.cpp
 *
 *  Created on: 09/05/2012
 *      Author: marrony
 */

#include "Scene.h"
#include "ResourceManager.h"
#include "Octree.h"
#include "Frustum.h"
#include "Geometry.h"
#include "SpatialLocation.h"
#include "GameEntity.h"
#include "Model.h"
#include "GeometryData.h"
#include "EventManager.h"
#include "Light.h"
#include "Node.h"
#include "SceneTree.h"
#include "Material.h"
#include "Texture.h"
#include "Image.h"
#include "Shader.h"
#include "Stream.h"

#include "Exception.h"

#include "MemoryManager.h"

namespace engine {

	const Type Scene::TYPE("scene");

	Scene::Scene(const std::string& name, ResourceManager* manager) :
			Resource(name, manager), atualCamera(0), sceneTree(0), root(0) {
	}

	template<typename T>
	static void eraseVector(std::vector<T*>& vector) {
		for(size_t i = 0; i < vector.size(); i++) {
			delete vector[i];
		}

		vector.clear();
	}

	Scene::~Scene() {
		delete sceneTree;

		eraseVector(cameras);
		eraseVector(spotLights);
		eraseVector(pointLights);
		eraseVector(geometries);
		eraseVector(nodes);
	}

	std::vector<Geometry*> Scene::getVisibleGeometries(const Frustum* const camera) {
		std::vector<Geometry*> visibleGeometries;

		QueryList queryList;
		queryList.setFrustum(camera);
		queryList.setFilter(SceneTreeType::GeometrySceneType);

		sceneTree->queryObjects(queryList);
		for(size_t i = 0; i < queryList.size(); i++) {
			Geometry* geometry = (Geometry*) queryList.getObject(i);
			visibleGeometries.push_back(geometry);
		}

		return visibleGeometries;
	}

	std::vector<Light*> Scene::getVisibleLights(const Frustum* const camera) {
		std::vector<Light*> visibleGeometries;

		QueryList queryList;
		queryList.setFrustum(camera);
		queryList.setFilter(SceneTreeType::LightSceneType);

		sceneTree->queryObjects(queryList);
		for(size_t i = 0; i < queryList.size(); i++) {
			Light* light = (Light*) queryList.getObject(i);
			if(light->isEnabled())
				visibleGeometries.push_back(light);
		}

		return visibleGeometries;
	}

	void Scene::updateRenderQueue(Frustum* camera) {
		QueryList queryList;
		queryList.setFrustum(camera);
		queryList.setFilter(SceneTreeType::GeometrySceneType);

		sceneTree->queryObjects(queryList);

		renderQueue.clear();

		for(size_t i = 0; i < queryList.size(); i++) {
			Geometry* geometry = (Geometry*) queryList.getObject(i);

			Model* model = geometry->getModel();

			for(size_t i = 0; i < model->geometryData->batches.size(); ++i) {
				RenderQueueEntry entry;

				entry.geometry = geometry;
				entry.material = model->geometryData->materials[i];
				entry.batch = &model->geometryData->batches[i];

				renderQueue.push_back(entry);
			}
		}
	}

	Node* Scene::createNode() {
		Node* node = new Node;

		nodes.push_back(node);

		return node;
	}

	Node* Scene::getRoot() {
		if(!root) root = createNode();

		return root;
	}

	Geometry* Scene::createGeometry(Model* model, const AABoundingBox& aabbox, Node* node) {
		if(!node)
			node = createNode();

		Geometry* geometry = new Geometry(model, aabbox, node);

		geometries.push_back(geometry);

		return geometry;
	}

	Light* Scene::createPointLight() {
		Node* node = createNode();

		Frustum* frustum = new Frustum(node);
		Light* light = new Light(LightType::POINT, frustum);

		light->getFrustum()->setPerspective(90, 1.0);
		pointLights.push_back(light);

		return light;
	}

	Light* Scene::createSpotLight() {
		Node* node = createNode();

		Frustum* frustum = new Frustum(node);
		Light* light = new Light(LightType::SPOT, frustum);

		light->getFrustum()->setPerspective(90, 1.0);
		spotLights.push_back(light);

		return light;
	}

	Frustum* Scene::createCamera(const std::string& name) {
		Frustum* frustum = new Frustum;
		frustum->setPerspective(60, 1);

		cameras.push_back(frustum);

		if(!atualCamera)
			atualCamera = frustum;

		return frustum;
	}

	Frustum* Scene::getActualCamera() {
		return atualCamera;
	}

	SceneTree* Scene::createSceneTree() {
		sceneTree = new Octree(math::Vector3(0, 0, 0), 100);
		sceneTree->maxLevel(10);
		sceneTree->maxObjects(10);
		sceneTree->minObjects(5);

		for(size_t i = 0; i < geometries.size(); i++) {
			sceneTree->addObject(geometries[i]);
		}

		for(size_t i = 0; i < pointLights.size(); i++) {
			sceneTree->addObject(pointLights[i]);
		}

		for(size_t i = 0; i < spotLights.size(); i++) {
			sceneTree->addObject(spotLights[i]);
		}

		return sceneTree;
	}

	void Scene::update(const GameTime& gameTime, ResourceManager& manager) {
		Frustum* frustum = atualCamera;
		math::Vector3 cameraPositon = frustum->getPosition();
		ObjectList queryList;

		sceneTree->queryObjects(queryList, cameraPositon, frustum->getFar() * 1.5);

		for(size_t i = 0; i < queryList.size(); ++i) {
			SceneTreeObject* obj = queryList.getObject(i);

			if(obj->getSceneTreeType() != SceneTreeType::GeometrySceneType)
				continue;

			Geometry* geometry = (Geometry*)obj;

			geometry->notifyVisibility(manager);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void* SceneUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		std::string type = stream.readString("type");
		std::string name = stream.readString("name");

		Scene* scene = new Scene(name, &manager);

		scene->nodes.resize(stream.readInt("nodesCount"));
		for(size_t i = 0; i < scene->nodes.size(); ++i) {
			Node* node = new Node;

			scene->nodes[i] = node;

			stream.pushGroup("node");

			math::Quaternion orientation;
			math::Vector3 position, scale;

			stream.readArray("orientation", orientation.quat, 4);
			stream.readArray("position", position.vector, 3);
			stream.readArray("scale", scale.vector, 3);

			node->setOrientation(orientation);
			node->setPosition(position);
			node->setScale(scale);

			int parentIndex = stream.readInt("parent");
			if(parentIndex != -1)
				node->setParent(scene->nodes[parentIndex]);

			stream.popGroup();
		}

		scene->root = scene->nodes[0];

		scene->geometries.resize(stream.readInt("geometriesCount"));
		for(size_t i = 0; i < scene->geometries.size(); ++i) {

			stream.pushGroup("geometry");

			std::string modelName = stream.readString("model");
			int nodeIndex = stream.readInt("modifier");
			Node* node = nodeIndex != -1 ? scene->nodes[nodeIndex] : 0;

			math::Vector3 min, max;

			stream.readArray("aabboxMin", min.vector, 3);
			stream.readArray("aabboxMax", max.vector, 3);

			Model* model = (Model*)manager.loadResource(ModelKey(modelName));
			Geometry* geometry = new Geometry(model, AABoundingBox(min, max), node);

			scene->geometries[i] = geometry;

			stream.popGroup();
		}

		scene->pointLights.resize(stream.readInt("pointLightsCount"));
		for(size_t i = 0; i < scene->pointLights.size(); ++i) {
			stream.pushGroup("pointLight");

			math::Vector3 attenuations, specular, diffuse, ambient, direction, position;

			bool isEnabled = stream.readByte("enabled") != 0;
			bool castShadow = stream.readByte("castShadow") != 0;
			stream.readArray("attenuations", attenuations.vector, 3);
			stream.readArray("specular", specular.vector, 3);
			stream.readArray("diffuse", diffuse.vector, 3);
			stream.readArray("ambient", ambient.vector, 3);

			std::string type = stream.readString("type");
			stream.readArray("direction", direction.vector, 3);
			stream.readArray("position", position.vector, 3);
			float znear = stream.readFloat("znear");
			float zfar = stream.readFloat("zfar");
			float aspect = stream.readFloat("aspect");
			float fov = stream.readFloat("fov");
			float left = stream.readFloat("left");
			float right = stream.readFloat("right");
			float top = stream.readFloat("top");
			float bottom = stream.readFloat("bottom");
			int modifier = stream.readInt("modifier");

			Frustum* frustum = new Frustum(scene->nodes[modifier]);

			if(type == "ortho") {
				frustum->setOrtho(left, right, bottom, top, znear, zfar);
			}

			if(type == "perspective") {
				frustum->setPerspective(fov, aspect, znear, zfar);
			}

			scene->pointLights[i] = new Light(LightType::POINT, frustum);
			if(isEnabled)
				scene->pointLights[i]->enable();
			scene->pointLights[i]->castShadow(castShadow);
			scene->pointLights[i]->setAttenuations(attenuations);
			scene->pointLights[i]->setSpecular(specular);
			scene->pointLights[i]->setDiffuse(diffuse);
			scene->pointLights[i]->setAmbient(ambient);

			stream.popGroup();
		}

		return scene;
	}

	void SceneUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Scene* scene = dynamic_cast<Scene*>((Scene*) instance);

		if(!scene)
			throw Exception("Resource is not a Scene");

		std::function<int(Node*)> getIndex = [&](Node* node) {
			auto parent = std::find(scene->nodes.begin(), scene->nodes.end(), node);

			if(parent != scene->nodes.end())
				return std::distance(scene->nodes.begin(), parent);

			return -1;
		};

		stream.writeString("type", scene->getType().getName());
		stream.writeString("name", scene->name);

		stream.writeInt("nodesCount", scene->nodes.size());
		for(size_t i = 0; i < scene->nodes.size(); ++i) {
			Node* node = scene->nodes[i];

			stream.pushGroup("node");

			stream.writeArray("orientation", node->getOrientation().quat, 4);
			stream.writeArray("position", node->getPosition().vector, 3);
			stream.writeArray("scale", node->getScale().vector, 3);
			stream.writeInt("parent", getIndex(node->getParent()));

			stream.popGroup();
		}

		stream.writeInt("geometriesCount", scene->geometries.size());
		for(size_t i = 0; i < scene->geometries.size(); ++i) {
			Geometry* geometry = scene->geometries[i];

			stream.pushGroup("geometry");

			Model* model = geometry->getModel();

			stream.writeString("model", model->getName());
			stream.writeInt("modifier", getIndex((Node*)geometry->getModifier()));

			AABoundingBox aabbox = geometry->getNonTransformedBoundingBox();
			stream.writeArray("aabboxMin", aabbox.getMin().vector, 3);
			stream.writeArray("aabboxMax", aabbox.getMax().vector, 3);

			stream.popGroup();
		}

		stream.writeInt("pointLightsCount", scene->pointLights.size());
		for(size_t i = 0; i < scene->pointLights.size(); ++i) {
			Light* light = scene->pointLights[i];

			stream.pushGroup("pointLight");

			stream.writeByte("enabled", light->isEnabled());
			stream.writeByte("castShadow", light->castShadow());
			stream.writeArray("attenuations", light->getAttenuations().vector, 3);
			stream.writeArray("specular", light->getSpecular().vector, 3);
			stream.writeArray("diffuse", light->getDiffuse().vector, 3);
			stream.writeArray("ambient", light->getAmbient().vector, 3);

			Frustum* frustum = light->getFrustum();
			FrustumType type = frustum->getType();

			switch(type) {
			case FrustumType::ORTHO:
				stream.writeString("type", "ortho");
				break;

			case FrustumType::PERSPECTIVE:
				stream.writeString("type", "perspective");
				break;

			default:
				stream.writeString("type", "unknow");
				break;
			}

			stream.writeArray("direction", frustum->getDirection().vector, 3);
			stream.writeArray("position", frustum->getPosition().vector, 3);
			stream.writeFloat("znear", frustum->getNear());
			stream.writeFloat("zfar", frustum->getFar());
			stream.writeFloat("aspect", frustum->getAspect());
			stream.writeFloat("fov", frustum->getFov());
			stream.writeFloat("left", frustum->getLeft());
			stream.writeFloat("right", frustum->getRight());
			stream.writeFloat("top", frustum->getTop());
			stream.writeFloat("bottom", frustum->getBottom());
			stream.writeInt("modifier", getIndex((Node*)frustum->getModifier()));

			stream.popGroup();
		}
	}

	Resource* SceneKey::loadResource(ResourceManager& manager) const {
		std::string sceneName = getName();

		FileStream fileStream("resources/scene/" + sceneName + ".scene");
		ResourceBinStream resourceStream(fileStream);
		Scene* scene = (Scene*)SceneUtils::read(resourceStream, manager, 0);

		return scene;
	}

} /* namespace engine */
