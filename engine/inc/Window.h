/*
 * Window.h
 *
 *  Created on: 02/09/2011
 *      Author: marrony.neris
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>

#include "TypedEvent.h"
#include "KeyEvent.h"

namespace engine {

	struct ResizeEvent {
		int width;
		int height;

		ResizeEvent(int width, int height) :
			width(width), height(height) { }
	};

	//	TODO: Criar subclasse com as especializacoes para windows
	class Window {
	public:
		TypedEvent<KeyEvent> onKeyUp;
		TypedEvent<KeyEvent> onKeyDown;
		TypedEvent<ResizeEvent> onResize;

		virtual ~Window() { };

		virtual void setTitle(const std::string& title) = 0;
		virtual void resize(int newWidth, int newHeight) = 0;
		virtual void show() = 0;
		virtual void hide() = 0;

		virtual bool processMessages() = 0;
	};

} /* namespace engine */
#endif /* WINDOW_H_ */
