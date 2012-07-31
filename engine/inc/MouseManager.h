#ifndef MOUSEMANAGER_H_
#define MOUSEMANAGER_H_

#include <vector>

#include "definitions.h"
#include "TypedEvent.h"

namespace engine {

	struct MouseState {
		enum KeyState {
			PRESSED,
			DOUBLE_PRESSED,
			RELESEAD
		};

		int x;
		int y;
		KeyState leftKey;
		KeyState rightKey;
		KeyState middleKey;

		MouseState(int x = 0, int y = 0, KeyState leftKey = MouseState::RELESEAD, KeyState rightKey = MouseState::RELESEAD, KeyState middleKey = MouseState::RELESEAD) :
				x(x), y(y), leftKey(leftKey), rightKey(rightKey), middleKey(middleKey) { }

		bool operator==(const MouseState& other) const {
			return x == other.x &&
					y == other.y &&
					leftKey == other.leftKey &&
					rightKey == other.rightKey &&
					middleKey == other.middleKey;
		}
	};

	struct MouseEvent {
		MouseState mouseState;

		MouseEvent(MouseState mouseState) : mouseState(mouseState) { }

		bool operator==(const MouseEvent& other) const {
			return mouseState == other.mouseState;
		}
	};

	class MouseManager {
	public:
		TypedEvent<MouseEvent> onMouseMove;
		TypedEvent<MouseEvent> onMouseDown;
		TypedEvent<MouseEvent> onMouseUp;
		TypedEvent<MouseEvent> onDoubleClick;

		VIRTUAL void setMousePosition(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			onMouseMove(MouseEvent(getMouseState()));
		}

		VIRTUAL void mouseLeftDown(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			mouseState.leftKey = MouseState::PRESSED;
			onMouseDown(MouseEvent(getMouseState()));
		}

		VIRTUAL void mouseLeftUp(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			mouseState.leftKey = MouseState::RELESEAD;
			onMouseUp(MouseEvent(getMouseState()));
		}

		VIRTUAL void mouseLeftDoubleClick(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			mouseState.leftKey = MouseState::DOUBLE_PRESSED;
			onDoubleClick(MouseEvent(getMouseState()));
		}

		VIRTUAL void mouseRightDown(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			mouseState.rightKey = MouseState::PRESSED;
			onMouseDown(MouseEvent(getMouseState()));
		}

		VIRTUAL void mouseRightUp(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			mouseState.rightKey = MouseState::RELESEAD;
			onMouseUp(MouseEvent(getMouseState()));
		}

		VIRTUAL void mouseRightDoubleClick(int x, int y) {
			mouseState.x = x;
			mouseState.y = y;
			mouseState.rightKey = MouseState::DOUBLE_PRESSED;
			onDoubleClick(MouseEvent(getMouseState()));
		}

		VIRTUAL MouseState getMouseState() {
			return mouseState;
		}
	private:
		MouseState mouseState;
	};

}

#endif /* MOUSEMANAGER_H_ */
