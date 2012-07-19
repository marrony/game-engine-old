/*
 * MyGame.cpp
 *
 *  Created on: 29/04/2012
 *      Author: Marrony
 */

#include "MyGame.h"

#include "Application.h"
#include "ResourceManager.h"
#include "EventManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "ControllerManager.h"
#include "KeyboardManager.h"

#include "Node.h"
#include "Model.h"
#include "Light.h"
#include "Material.h"
#include "Frustum.h"
#include "Geometry.h"
#include "Scene.h"
#include "SpatialLocation.h"
#include "GameEntity.h"

#include "MyImpls.h"
#include "SimpleRenderer.h"

#include <iostream>

#include "MemoryManager.h"

using namespace engine;

class Animator : public engine::ControllerComponent {
public:
	Animator(Geometry* geometry, engine::EventManager& eventManager, float frame) :
			geometry(geometry), eventManager(eventManager), frame(frame) {
	}

	virtual void updateBindings(engine::GameEntity* owner) {
		geometry = owner->getComponent<engine::Geometry>();
	}

	virtual void update(const engine::GameTime& gameTime) {
		frame += 10 * gameTime.elapsedTimeInSec;
		geometry->setFrame(frame);

		if(frame >= 30)
			frame = 0;
	}

private:
	engine::EventManager& eventManager;
	engine::Geometry* geometry;
	float frame;
};

class Event1 : public engine::Event {
public:
	Event1(engine::Type type) : Event(type) { }

	static const engine::Type type;
};

const engine::Type Event1::type("on");

class Event2 : public engine::Event {
public:
	Event2(engine::Type type) : Event(type) { }

	static const engine::Type type;
};

const engine::Type Event2::type("off");

MyGame::~MyGame() {}

void MyGame::create(Application& app) {
	application = &app;

	renderer = new SimpleRenderer(application->getResourceManager());

//		getEventManager().registerHandler(Event1::type, [&](void* sender, Event* event) {
//			onEvent1(sender, event);
//		});
//
//		getEventManager().registerHandler(Event2::type, [&](void* sender, Event* event) {
//			onEvent2(sender, event);
//		});

	Event* event1 = application->getEventManager().createEvent<Event1>();
	application->getEventManager().postEvent(event1, 2.0);

	Node* node;
	Geometry* geometry;

	scene = application->getResourceManager().loadScene("large-scene");

//	ResourceId modelId = application->getResourceManager().registerResource("duck_triangulate", Model::TYPE);
//	Model* model = (Model*)application->getResourceManager().getResource(modelId);
//
//	node = scene->createNode();
//	node->setPosition(math::Vector3(0, 0, 25));
//	node->setScale(math::Vector3(0.1, 0.1, 0.1));
//	node->setOrientation(math::Quaternion(math::AxisAngle(3.14, math::Vector3(0, 1, 0))));
//	scene->getRoot()->addChild(node);
//	geometry = scene->createGeometry(modelId, model->getBoundingBox(), node);

#if 1
	ptLight0 = scene->createSpotLight();
	ptLight0->setAmbient(math::Vector3(1.0, 1.0, 0.5));
	ptLight0->setDiffuse(math::Vector3(1.0, 1.0, 1.0));
	ptLight0->setSpecular(math::Vector3(0.5, 0.5, 0.5));
//		ptLight0->setPosition(math::Vector3(0.0, 60.0, 0.0));
	ptLight0->setAttenuations(math::Vector3(250.0, 1.0, 1.0));
	ptLight0->setDirection(math::Vector3(0.0, -1.0, 0.0).normalize());
	ptLight0->castShadow(true);

	node = (Node*)ptLight0->getFrustum()->getModifier();
	node->setPosition(math::Vector3(0.0, 40.0, 0.0));
	scene->getRoot()->addChild(node);
#endif

	scene->createSceneTree();

	Frustum* myCamera = scene->createCamera("camera");
	myCamera->setFar(50.0);
	application->getControllerManager().addController(new MyCameraController(myCamera, &application->getKeyboardManager(), 90));
}

void MyGame::destroy() {
	application->getResourceManager().unloadScene(scene);

	delete renderer;
}

void MyGame::update(const GameTime& gameTime) {
	scene->update(gameTime, application->getResourceManager());
}

void MyGame::render() {
	renderer->render(&application->getGraphicManager(), scene);
}

void MyGame::onEvent1(void* sender, Event* event) {
	std::cout << event->getType().getName() << std::endl;

	Event* event2 = application->getEventManager().createEvent<Event2>();
	application->getEventManager().postEvent(event2, 1000.0);

	float r = rand() % 255;
	float g = rand() % 255;
	float b = rand() % 255;

	ptLight0->setDiffuse(math::Vector3(r / 255, g / 255, b / 255));
	//ptLight0->enable();
}

void MyGame::onEvent2(void* sender, Event* event) {
	std::cout << event->getType().getName() << std::endl;

	Event* event1 = application->getEventManager().createEvent<Event1>();
	application->getEventManager().postEvent(event1, 300.0);

	//ptLight0->disable();
}
