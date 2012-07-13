/*
 * TypedEvent.h
 *
 *  Created on: 02/09/2011
 *      Author: marrony.neris
 */

#ifndef TYPEDEVENT_H_
#define TYPEDEVENT_H_

#include <vector>
#include <functional>
#include <algorithm>

namespace engine {

	template<typename T>
	class TypedEvent {
		std::vector<std::function<void(void*, T)>> handlers;
	public:
		TypedEvent() : handlers() { }

		void operator+=(std::function<void(void*, T)> eventHandler) {
			handlers.push_back(eventHandler);
		}

		void operator-=(std::function<void(void*, T)> eventHandler) {
			auto ite = std::find(handlers.begin(), handlers.end(), eventHandler);

			if(ite != handlers.end()) {
				handlers.erase(ite);
			}
		}

		void operator()(void* sender, T event) {
			for(auto& h : handlers) {
				h(sender, event);
			}
		}
	};

} // namespace engine

#endif /* TYPEDEVENT_H_ */
