/*
 * EventManager.h
 *
 *  Created on: 31/10/2011
 *      Author: marrony
 */

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <functional>
#include <queue>
#include <set>
#include <map>

#include "Clock.h"
#include "Type.h"
#include "EventQueueHandler.h"
#include "TypedEvent.h"

namespace engine {

	class Event {
	public:
		Event(Type type);

		Type getType() const;
	private:
		Type type;
	};

	class EventQueue {
	public:
		struct TimedEvent {
			Event* event;
			float timer;

			bool operator<(const TimedEvent& other) const {
				return other.timer < timer;
			}
		};

		void pushEvent(Event* event, float timer);

		const TimedEvent& peekEvent() const;

		void popEvent();

		bool isEmpty() const;
	private:
		std::priority_queue<TimedEvent> queue;
	};

	class EventManager {
	public:
		EventManager(const Clock& clock);
		~EventManager();

		void eraseAllEvents();

		template<typename T>
		T* createEvent() {
			Type type = T::type;
			T* event = 0;

			EventCache::iterator ite = eventCache.find(type);
			if(ite == eventCache.end()) {
				event = new T(type);

				allEvents.insert(event);
			} else {
				event = (T*) ite->second;
				eventCache.erase(ite);
			}

			return event;
		}

		bool deleteEvent(Event* event);

		bool postEvent(Event* event, float time);

		void sendEvent(Event* event);

		void updateEvents();

		void registerHandler(Type type, std::function<void(void*, Event*)> handler);

		void unregisterHandler(Type type, std::function<void(void*, Event*)> handler);

		void unregisterHandler(std::function<void(void*, Event*)> handler);
	private:
		typedef std::vector<std::function<void(void*, Event*)>> HandlerSet;
		typedef std::map<Type, HandlerSet> HandlerMap;
		typedef std::multimap<Type, Event*> EventCache;
		typedef std::set<Event*> EventSet;

		HandlerMap handlers;
		EventQueue queue[2];
		int readQueueIndex;
		int writeQueueIndex;

		EventCache eventCache;
		EventSet allEvents;

		const Clock& clock;
	};

} /* namespace engine */

#endif /* EVENTMANAGER_H_ */
