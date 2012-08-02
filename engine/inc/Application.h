/*
 * Application.h
 *
 *  Created on: 02/09/2011
 *      Author: marrony.neris
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "TypedEvent.h"
#include "Clock.h"

namespace engine {

	class Window;
	class GPUComputing;
	class EventManager;
	class GraphicManager;
	class SceneManager;
	class ResourceManager;
	class ControllerManager;
	class KeyboardManager;
	class MouseManager;
	class TextureManager;

	class Game;
	class GameTime;

	class Application {
	public:
		Application();
		virtual ~Application();

		const Clock& getClock() const {
			return clock;
		}

		EventManager& getEventManager() const {
			return *eventManager;
		}

		ResourceManager& getResourceManager() const {
			return *resourceManager;
		}

		SceneManager& getSceneManager() const {
			return *sceneManager;
		}

		GraphicManager& getGraphicManager() const {
			return *graphicManager;
		}

		ControllerManager& getControllerManager() const {
			return *controllerManager;
		}

		KeyboardManager& getKeyboardManager() const {
			return *keyboardManager;
		}

		void quit();
		void run(Game& game);

		virtual void createApplication();
		virtual void destroyApplication();
	protected:
		virtual bool processMessages();
		void swapBuffers();

		Clock clock;
		bool running;

		GPUComputing* gpuComputing;

		EventManager* eventManager;
		GraphicManager* graphicManager;
		SceneManager* sceneManager;
		ResourceManager* resourceManager;
		ControllerManager* controllerManager;
		KeyboardManager* keyboardManager;
		MouseManager* mouseManager;
		TextureManager* textureManager;
	};

} /* namespace engine */

#endif /* APPLICATION_H_ */
