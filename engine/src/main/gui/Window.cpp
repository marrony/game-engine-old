/*
 * Window.cpp
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#include "gui/Window.h"

namespace engine {

	namespace gui {

		Window::Window(const std::string& title, const Rect& size, KeyboardManager& keyboardManager, MouseManager& mouseManager) :
			Container(size, keyboardManager, mouseManager), title(title) {

			color[0] = 0xff;
			color[1] = 0xff;
			color[2] = 0xff;
			color[3] = 0xff;
		}

//		void enter2D(float x, float y, float width, float height) {
//			glMatrixMode(GL_PROJECTION);
//			glPushMatrix();
//			glLoadIdentity();
//			glOrtho(x, width, y, height, -1, +1);
//
//			glMatrixMode(GL_MODELVIEW);
//			glPushMatrix();
//			glLoadIdentity();
//		}
//
//		void leave2D() {
//			glMatrixMode(GL_PROJECTION);
//			glPopMatrix();
//
//			glMatrixMode(GL_MODELVIEW);
//			glPopMatrix();
//		}

		void Window::draw() {
#if 0
			float x = size.getX();
			float y = size.getY();
			float width = size.getWidth();
			float height = size.getHeight();

			enter2D(0, 0, 400, 400);

			glDepthFunc(GL_ALWAYS);

//			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_SRC_ALPHA);

//			glEnable(GL_ALPHA_TEST);
//			glAlphaFunc(GL_GREATER, 0.5);

			glColor4ubv(color);

			glBegin(GL_QUADS);

				glVertex2f(x, y);
				glVertex2f(x+width, y);
				glVertex2f(x+width, y+height);
				glVertex2f(x, y+height);

			glEnd();

			ControlList::iterator ite;
			for(ite = children.begin(); ite != children.end(); ite++) {
				Control* control = ite->get();
				control->draw();
			}

			glDisable(GL_BLEND);
//			glDisable(GL_ALPHA_TEST);

			leave2D();
#endif
		}

	}  // namespace gui

}  // namespace engine
