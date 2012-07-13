/*
 * Event.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#ifndef EVENT_H_
#define EVENT_H_


namespace engine {

	namespace gui {

		class Event {
		public:
			static const int KEYBOARD    = 1;
			static const int MOUSE_MOVE  = 2;
			static const int MOUSE_CLICK = 3;

			int type;
			int x;
			int y;
			int buttom;
			bool click;
		};

		class ResizeEvent {
		public:
			bool operator==(const ResizeEvent& other) const {
				return true;
			}
		};

	}  // namespace gui

}  // namespace engine

#endif /* EVENT_H_ */
