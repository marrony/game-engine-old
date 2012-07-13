/*
 * KeyboardManager.h
 *
 *  Created on: 04/03/2011
 *      Author: marrony.neris
 */

#ifndef KEYBOARDMANAGER_H_
#define KEYBOARDMANAGER_H_

#include <vector>
#include <memory.h>

#include "definitions.h"
#include "TypedEvent.h"
#include "KeyEvent.h"

namespace engine {

	class KeyboardManager {
	public:
		TypedEvent<KeyEvent> onKeyUp;
		TypedEvent<KeyEvent> onKeyDown;

		KeyboardManager() {
			memset(keysPressed, 0, sizeof(keysPressed));
		}

		VIRTUAL void keyUp(int key) {
			keysPressed[key] = false;

			onKeyUp(this, KeyEvent(key));
		}

		VIRTUAL void keyDown(int key) {
			keysPressed[key] = true;

			onKeyDown(this, KeyEvent(key));
		}

		VIRTUAL bool isKeyPressed(int key) {
			return keysPressed[key];
		}

	private:
		bool keysPressed[0xff];
	};
}

#endif /* KEYBOARDMANAGER_H_ */
