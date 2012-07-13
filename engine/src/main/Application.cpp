/*
 * Application.cpp
 *
 *  Created on: 02/09/2011
 *      Author: marrony.neris
 */

#include "Application.h"
#include "EventManager.h"
#include "Window.h"
#include "GraphicManager.h"
#include "ControllerManager.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Game.h"

namespace engine {

	Application::Application() :
			clock(), running(false) {
	}

	Application::~Application() {
	}

	void Application::quit() {
		running = false;
	}

/**
 * while (true) { // main game loop
 * 	// ...
 *
 * 	for (each gameObject) {
 * 		gameObject.PreAnimUpdate(dt);
 * 	}
 *
 * 	g_animationEngine.CalculateIntermediatePoses(dt);
 *
 * 	for (each gameObject) {
 * 		gameObject.PostAnimUpdate(dt);
 * 	}
 *
 * 	g_ragdollSystem.ApplySkeletonsToRagDolls();
 *
 * 	g_physicsEngine.Simulate(dt); // runs ragdolls too
 *
 *		g_collisionEngine.DetectAndResolveCollisions(dt);
 *
 * 	g_ragdollSystem.ApplyRagDollsToSkeletons();
 *
 * 	g_animationEngine.FinalizePoseAndMatrixPalette();
 *
 * 	for (each gameObject) {
 * 		gameObject.FinalUpdate(dt);
 * 	}
 * }
 */
	void Application::run(Game& game) {
		createApplication();

		double time = 1.0 / 60.0;
		clock.start(- time * 1000000.0);

		running = true;

		game.create(*this);

		while(running) {
			if(!processMessages())
				break;

			clock.tick();

			GameTime gameTime(clock.getElapsedTimeInMicroSec(), clock.getFps());

			eventManager->updateEvents();

			controllerManager->update(gameTime);

			game.update(gameTime);
			game.render();

			swapBuffers();
		}

		game.destroy();

		destroyApplication();
	}

	bool Application::processMessages() {
	}

	void Application::swapBuffers() {
		graphicManager->swapBuffers();
	}

	void Application::createApplication() {
	}

	void Application::destroyApplication() {
	}

} /* namespace engine */
