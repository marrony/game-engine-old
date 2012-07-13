/*
 * Window.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include "gui/Container.h"

#include <string>

namespace engine {

	namespace gui {

		class Window : public Container {
			std::string title;
			unsigned char color[4];
		public:
			Window(const std::string& title, const Rect& size, KeyboardManager& keyboardManager, MouseManager& mouseManager);

			virtual void draw();
		};

	}  // namespace gui

}  // namespace engine

#endif /* WINDOW_H_ */
