/*
 * EventQueueHandler.h
 *
 *  Created on: 31/10/2011
 *      Author: marrony
 */

#ifndef EVENTQUEUEHANDLER_H_
#define EVENTQUEUEHANDLER_H_

namespace engine {

	class Event;

	class EventQueueHandler {
	public:
		virtual ~EventQueueHandler() { }

		virtual bool handleEvent(Event* event) = 0;
	};

} // namespace engine

#endif /* EVENTQUEUEHANDLER_H_ */
