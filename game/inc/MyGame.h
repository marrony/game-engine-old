/*
 * MyGame.h
 *
 *  Created on: 29/04/2012
 *      Author: Marrony
 */

#ifndef MYGAME_H_
#define MYGAME_H_

#include "Game.h"

namespace engine {
	class Application;
	class Renderer;
	class Light;
	class Event;
	class Scene;
}

class MyGame : public engine::Game {
	engine::Application* application;
	engine::Renderer* renderer;
	engine::Scene* scene;
	engine::Light* ptLight0;
public:
	virtual ~MyGame();

	virtual void create(engine::Application& app);

	virtual void destroy();

	virtual void update(const engine::GameTime& gameTime);

	virtual void render();

	void onEvent1(void* sender, engine::Event* event);

	void onEvent2(void* sender, engine::Event* event);
};

#endif /* MYGAME_H_ */
