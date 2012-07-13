/*
 * EventManager.cpp
 *
 *  Created on: 31/10/2011
 *      Author: marrony
 */

#include "EventManager.h"

namespace engine {

	Event::Event(Type type) :
			type(type) {
	}

	Type Event::getType() const {
		return type;
	}

	void EventQueue::pushEvent(Event* event, float timer) {
		TimedEvent te = { event, timer };

		queue.push(te);
	}

	const EventQueue::TimedEvent& EventQueue::peekEvent() const {
		return queue.top();
	}

	void EventQueue::popEvent() {
		queue.pop();
	}

	bool EventQueue::isEmpty() const {
		return queue.empty();
	}

	EventManager::EventManager(const Clock& clock) :
			readQueueIndex(0), writeQueueIndex(1), clock(clock) {
	}

	EventManager::~EventManager() {
		eraseAllEvents();
	}

	void EventManager::eraseAllEvents() {
		for(EventSet::iterator ite = allEvents.begin(); ite != allEvents.end(); ite++)
			delete *ite;
	}

	bool EventManager::deleteEvent(Event* event) {
		if(allEvents.count(event) == 0)
			return false;

		eventCache.insert(std::make_pair(event->getType(), event));
		return true;
	}

	bool EventManager::postEvent(Event* event, float time) {
//		Type type = event->getType();
//		HandlerMap::iterator handlerSet = handlers.find(type);
//
//		if(handlerSet == handlers.end())
//			return false;

		queue[writeQueueIndex].pushEvent(event, clock.getTimeInMicroSec() + time * 1000.0);
		return true;
	}

	void EventManager::sendEvent(Event* event) {
		Type type = event->getType();

		HandlerSet& handlerSet = handlers[type];

		for(auto& delegate : handlerSet) {
			delegate(this, event);
		}
	}

	void EventManager::updateEvents() {
		readQueueIndex = (readQueueIndex + 1) % 2;
		writeQueueIndex = (writeQueueIndex + 1) % 2;

		EventQueue& readQueue = queue[readQueueIndex];

		double time = clock.getTimeInMicroSec();

		while(!readQueue.isEmpty()) {
			const EventQueue::TimedEvent& timedEvent = readQueue.peekEvent();

			if(timedEvent.timer > time)
				break;

			sendEvent(timedEvent.event);
			deleteEvent(timedEvent.event);

			readQueue.popEvent();
		}
	}

	void EventManager::registerHandler(Type type, std::function<void(void*, Event*)> handler) {
		handlers[type].push_back(handler);
	}

	void EventManager::unregisterHandler(Type type, std::function<void(void*, Event*)> handler) {
//		HandlerMap::iterator ite = handlers.find(type);
//
//		if(ite != handlers.end()) {
//			HandlerSet::iterator h = std::remove(ite->second.begin(), ite->second.end(), handler);
//			ite->second.erase(h, ite->second.end());
//		}
	}

	void EventManager::unregisterHandler(std::function<void(void*, Event*)> handler) {
		for(HandlerMap::iterator ite = handlers.begin(); ite != handlers.end(); ite++)
			unregisterHandler(ite->first, handler);
	}

} /* namespace engine */
