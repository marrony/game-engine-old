/*
 * MyImpls.h
 *
 *  Created on: 28/10/2010
 *      Author: marrony.neris
 */

#ifndef MYIMPLS_H_
#define MYIMPLS_H_

#include "System.h"
#include "Texture.h"
#include "Shader.h"
#include "GeometricMeshImpl.h"
#include "Exception.h"
#include "Light.h"
#include "ControllerManager.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "GraphicManager.h"
#include "Node.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "Frustum.h"
#include "GameEntity.h"
#include "Clock.h"

class MyLightController : public engine::ControllerComponent {
	engine::Light* light;
	float angle;
public:
	MyLightController(engine::Light* _light) :
		light(_light), angle(0) {
	}

	virtual void update(const engine::GameTime& gameTime) {
		angle += gameTime.elapsedTimeInSec;

		float dirX = cos(angle) * 50.0;
		float dirY = 0;
		float dirZ = sin(angle) * 50.0;

		engine::math::Vector3 position(dirX, dirY, dirZ);

		light->setDirection((position - light->getPosition()).normalize());
	}
};

class MyCameraController : public engine::ControllerComponent {
	engine::Frustum* frustum;
	engine::KeyboardManager* keyboardManager;
	float angle;
public:
	MyCameraController(engine::Frustum* newCamera, engine::KeyboardManager* keyboardManager, float angle = 90.0) :
		frustum(newCamera), keyboardManager(keyboardManager), angle(angle*3.14159/180.0) {
	}

	virtual void update(const engine::GameTime& gameTime) {

		if(keyboardManager->isKeyPressed('a') || keyboardManager->isKeyPressed('A')) {
			angle -= gameTime.elapsedTimeInSec;
		}

		if(keyboardManager->isKeyPressed('d') || keyboardManager->isKeyPressed('D')) {
			angle += gameTime.elapsedTimeInSec;
		}

		float dirX = cos(angle);
		float dirY = 0;
		float dirZ = sin(angle);

		engine::math::Vector3 position = frustum->getPosition();

#ifdef WIN32
		if(keyboardManager->isKeyPressed(VK_UP)) {
			position.y += 10.0*gameTime.elapsedTimeInSec;
		}

		if(keyboardManager->isKeyPressed(VK_DOWN)) {
			position.y -= 10.0*gameTime.elapsedTimeInSec;
		}
#endif

		frustum->setDirection(engine::math::Vector3(dirX, dirY, dirZ));

		float dir = 0;
		if(keyboardManager->isKeyPressed('w') || keyboardManager->isKeyPressed('W')) {
			dir = +5;
		}

		if(keyboardManager->isKeyPressed('s') || keyboardManager->isKeyPressed('S')) {
			dir = -5;
		}

		position.x += dirX * dir * gameTime.elapsedTimeInSec;
		position.y += dirY * dir * gameTime.elapsedTimeInSec;
		position.z += dirZ * dir * gameTime.elapsedTimeInSec;
		frustum->setPosition(engine::math::Vector3(position.x, position.y, position.z));
	}
};

class MyController : public engine::ControllerComponent {
	engine::Node* node;
	engine::math::Vector3 axis;
public:
	MyController(engine::Node* newNode, const engine::math::Vector3& newAxis)
		: node(newNode), axis(newAxis) {
	}

	virtual void update(const engine::GameTime& gameTime) {
		engine::math::Quaternion orientacaoAntiga(node->getOrientation());
		engine::math::Quaternion delta(engine::math::AxisAngle(0.5*gameTime.elapsedTimeInSec, axis));

		node->setOrientation(orientacaoAntiga * delta);
	}
};

#endif /* MYIMPLS_H_ */
