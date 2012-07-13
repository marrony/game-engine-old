/*
 * Control.cpp
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */
#include "gui/Control.h"

namespace engine {

	namespace gui {

		Control::Control(const Rect& rect, KeyboardManager& keyboardManager, MouseManager& mouseManager) :
				parent(0), rect(rect), visible(true), isMouseOver(false), isPressed(false),
				keyboardManager(keyboardManager), mouseManager(mouseManager) {
		}

		Control::~Control() {
		}

		void Control::internalDraw() {
		}

		void Control::internalUpdate() {
			MouseState mouseState = mouseManager.getMouseState();

			isMouseOver = rect.contains(mouseState.x, mouseState.y);

			if(isMouseOver)
				onMouseOver(this, MouseEvent(mouseState));

			if(!isPressed) {
				if(mouseState.leftKey == MouseState::PRESSED && isMouseOver) {
					isPressed = true;
					onPressed(this, MouseEvent(mouseState));
				}
			} else {
				if(mouseState.leftKey == MouseState::RELESEAD) {
					isPressed = false;
					onReleased(this, MouseEvent(mouseState));
				}
			}
		}

		void Control::draw() {
			if(visible)
				internalDraw();

//			float x = size.getX();
//			float y = size.getY();
//			float width = size.getWidth();
//			float height = size.getHeight();
//
//			glColor3f(1, 0, 0);
//			glBegin(GL_QUADS);
//
//				glVertex2f(x, y);
//				glVertex2f(x+width, y);
//				glVertex2f(x+width, y+height);
//				glVertex2f(x, y+height);
//
//			glEnd();
		}

		void Control::update() {
			if(visible)
				internalUpdate();
		}

		void Control::resize(const Size& size) {
			rect.resize(size);

			onResize(this, ResizeEvent());
		}

		void Control::setParent(Control* parent) {
			this->parent = parent;
		}

		Control* Control::getParent() const {
			return parent;
		}

		Position Control::getPosition() const {
			return rect.getPosition();
		}

		Position Control::getAbsolutePosition() const {
			Position thisPosition = getPosition();

			if(parent) {
				Position parentPosition = parent->getAbsolutePosition();
				return Position(parentPosition.x + thisPosition.x, parentPosition.y + thisPosition.y);
			}

			return thisPosition;
		}

	}  // namespace gui

}  // namespace engine
