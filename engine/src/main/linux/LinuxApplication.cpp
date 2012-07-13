/*
 * LinuxApplication.cpp
 *
 *  Created on: 27/04/2012
 *      Author: objective
 */

#include "linux/LinuxApplication.h"
#include "linux/LinuxWindow.h"
#include "linux/LinuxGraphicManager.h"

#include "EventManager.h"
#include "Window.h"
#include "GraphicManager.h"
#include "ControllerManager.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"

#include "MemoryManager.h"

namespace engine {

	LinuxApplication::LinuxApplication() {
	}

	LinuxApplication::~LinuxApplication() {
	}

	void LinuxApplication::createApplication() {
		window = new LinuxWindow(400, 400);
		window->show();

		graphicManager = new LinuxGraphicManager(window);
		//gpuComputing = new OpenCL(graphicManager);

		eventManager = new EventManager(clock);
		controllerManager = new ControllerManager;
		keyboardManager = new KeyboardManager;
		mouseManager = new MouseManager;
		resourceManager = new ResourceManager;
		sceneManager = new SceneManager;

		window->onKeyUp += [=](void* sender, KeyEvent event) {
			keyboardManager->keyUp(event.keyCode);
		};

		window->onKeyDown += [=](void* sender, KeyEvent event) {
			keyboardManager->keyDown(event.keyCode);
		};

		window->onResize += [=](void* sender, ResizeEvent event) {
			graphicManager->resize(event.width, event.height);
		};
	}

	void LinuxApplication::destroyApplication() {
		delete sceneManager;
		delete resourceManager;
		delete mouseManager;
		delete keyboardManager;
		delete controllerManager;
		delete eventManager;

		//delete gpuComputing;
		delete graphicManager;
		delete window;
	}

	bool LinuxApplication::processMessages() {
		return window->processMessages();
	}

} /* namespace engine */
