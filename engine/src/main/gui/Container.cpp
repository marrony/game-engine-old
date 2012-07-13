/*
 * Container.cpp
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#include "gui/Container.h"

namespace engine {

	namespace gui {
		Container::Container(const Rect& size, KeyboardManager& keyboardManager, MouseManager& mouseManager) :
			Control(size, keyboardManager, mouseManager) {
		}

		Container::~Container() {
			for(std::size_t i = 0; i < children.size(); i++) {
				delete children[i];
			}

			children.clear();
		}

		void Container::internalDraw() {
		}

		void Container::internalUpdate() {
		}

		void Container::addChild(Control* child) {
			children.push_back(child);
		}

		void Container::removeChild(Control* child) {
			//TODO
		}

	}  // namespace gui

}  // namespace engine
