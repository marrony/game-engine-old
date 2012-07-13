/*
 * Container.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "gui/Control.h"

#include <vector>

namespace engine {

	namespace gui {

		typedef std::vector<Control*> ControlList;

		class Container : public Control {
		protected:
			ControlList children;

			virtual void internalDraw();
			virtual void internalUpdate();
		public:
			Container(const Rect& size, KeyboardManager& keyboardManager, MouseManager& mouseManager);
			virtual ~Container();

			virtual void addChild(Control* child);
			virtual void removeChild(Control* child);
		};

	}  // namespace gui

}  // namespace engine

#endif /* CONTAINER_H_ */
