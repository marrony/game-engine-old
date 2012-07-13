/*
 * Control.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include "gui/Rect.h"
#include "gui/Event.h"

#include "TypedEvent.h"
#include "MouseManager.h"
#include "KeyboardManager.h"

namespace engine {

	namespace gui {

		class Control {
		protected:
			Control* parent;
			Rect rect;
			bool visible;
			bool isMouseOver;
			bool isPressed;
			KeyboardManager& keyboardManager;
			MouseManager& mouseManager;

			virtual void internalDraw();
			virtual void internalUpdate();
		public:
			TypedEvent<ResizeEvent> onResize;
			TypedEvent<MouseEvent> onMouseOver;
			TypedEvent<MouseEvent> onPressed;
			TypedEvent<MouseEvent> onReleased;

			Control(const Rect& rect, KeyboardManager& keyboardManager, MouseManager& mouseManager);
			virtual ~Control();

			virtual void draw();
			virtual void update();
			virtual void resize(const Size& size);

			virtual void setParent(Control* parent);
			virtual Control* getParent() const;

			virtual Position getPosition() const;
			virtual Position getAbsolutePosition() const;
		};

	}  // namespace gui

}  // namespace engine

#endif /* CONTROL_H_ */
