/*
 * main.cpp
 *
 *  Created on: 28/07/2010
 *      Author: marrony
 */
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "SimpleRenderer.h"

#include "Application.h"
#include "Exception.h"
#include "Octree.h"
#include "GameEntity.h"
#include "SparseVoxelOctreeBuilder.h"
#include "SparseVoxelOctree.h"
#include "EventManager.h"
#include "GPUComputing.h"
#include "Win32Window.h"
#include "Win32GraphicManager.h"
//#include "OpenCL.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "ControllerManager.h"
#include "SceneManager.h"
#include "Renderer.h"

#include "gui/WindowManager.h"
#include "gui/Window.h"

#include "Unit.h"

class MyApplication : public engine::Application {
public:
	MyApplication() {
		createApplication();
		createScene();
	}

	virtual ~MyApplication() {
		destroyScene();
		destroyApplication();
	}

	void onKeyUp(void* sender, engine::KeyEvent event) {
		keyboardManager->keyUp(event.keyCode);
	}

	void onKeyDown(void* sender, engine::KeyEvent event) {
		keyboardManager->keyDown(event.keyCode);
	}

	void onResize(void* sender, engine::ResizeEvent event) {
		graphicManager->resize(event.width, event.height);
	}

protected:
	void createApplication() {
		window = new engine::Win32Window(400, 400);
		graphicManager = new engine::Win32GraphicManager((engine::Win32Window*) window);
		//gpuComputing = new engine::OpenCL(graphicManager);

		window->onKeyUp += [&](void* sender, engine::KeyEvent event) {
			onKeyUp(sender, event);
		};

		window->onKeyDown += [&](void* sender, engine::KeyEvent event) {
			onKeyDown(sender, event);
		};

		window->onResize += [&](void* sender, engine::ResizeEvent event) {
			onResize(sender, event);
		};
	}

	void destroyApplication() {
		//delete gpuComputing;
		delete graphicManager;
		delete window;
	}

	void createScene() {
		window->show();

		controllerManager = new engine::ControllerManager;
		keyboardManager = new engine::KeyboardManager;
		mouseManager = new engine::MouseManager;
		//sceneManager = new engine::SceneManager(getEventManager());
		//renderer = new DeferredLighting;
		renderer = new SimpleRenderAlgorithm;

		sceneManager->createSceneTree();
		sceneManager->createCamera("camera");

		windowManager = new engine::gui::WindowManager;
	}

	void destroyScene() {
		delete renderer;
		delete controllerManager;
		delete keyboardManager;
		delete mouseManager;
		delete sceneManager;
		delete windowManager;
	}

	virtual bool processMessages() {
		return window->processMessages();
	}

	virtual void swapBuffers() {
		graphicManager->swapBuffers();
	}

	virtual void update(const engine::GameTime& gameTime) {
		controllerManager->update(gameTime);
	}

	virtual void preRender() { }

	virtual void postRender() {
		windowManager->render(graphicManager);
	}

	virtual void render() {
		renderer->render(graphicManager, sceneManager);
	}

private:
	engine::Window* window;
	engine::GPUComputing* gpuComputing;

	engine::GraphicManager* graphicManager;
	engine::SceneManager* sceneManager;
	engine::Renderer* renderer;
	engine::ControllerManager* controllerManager;
	engine::KeyboardManager* keyboardManager;
	engine::MouseManager* mouseManager;

	engine::gui::WindowManager* windowManager;
};

#include "MemoryManager.h"

int main(int argv, char* argc[]) {
	char* str = new char;
	delete str;

	try {
		MyApplication app;
		app.run();
	} catch(const engine::Exception& ex) {
		std::cout << ex.getMessage() << std::endl;
	} catch(...) {
		std::cout << "execeção desconhecida" << std::endl;
	}
	return 0;
}
