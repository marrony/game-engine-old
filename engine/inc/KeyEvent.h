/*
 * KeyEvent.h
 *
 *  Created on: 20/10/2011
 *      Author: marrony
 */

#ifndef KEYEVENT_H_
#define KEYEVENT_H_

namespace engine {

	struct KeyEvent {
		int keyCode;

		KeyEvent(int keyCode) : keyCode(keyCode) { }

		bool operator==(const KeyEvent& other) const {
			return keyCode == other.keyCode;
		}
	};

}  // namespace engine


#endif /* KEYEVENT_H_ */
