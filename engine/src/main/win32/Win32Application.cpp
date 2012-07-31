/*
 * Win32Application.cpp
 *
 *  Created on: 27/04/2012
 *      Author: objective
 */

#include "win32/Win32Application.h"
#include "win32/Win32Window.h"
#include "win32/Win32GraphicManager.h"

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

	Win32Application::Win32Application() {
	}

	Win32Application::~Win32Application() {
	}

	void Win32Application::createApplication() {
		window = new Win32Window(400, 400);
		graphicManager = new Win32GraphicManager(window);
		//gpuComputing = new OpenCL(graphicManager);

		eventManager = new EventManager(clock);
		controllerManager = new ControllerManager;
		keyboardManager = new KeyboardManager;
		mouseManager = new MouseManager;
		resourceManager = new ResourceManager;
		sceneManager = new SceneManager;

		resourceManager->addListener(graphicManager);

		window->onKeyUp += [=](KeyEvent event) {
			keyboardManager->keyUp(event.keyCode);
		};

		window->onKeyDown += [=](KeyEvent event) {
			keyboardManager->keyDown(event.keyCode);
		};

		window->onResize += [=](ResizeEvent event) {
			graphicManager->resize(event.width, event.height);
		};

		window->show();
	}

	void Win32Application::destroyApplication() {
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

	bool Win32Application::processMessages() {
		return window->processMessages();
	}

} /* namespace engine */
