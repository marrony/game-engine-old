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

	class Connection {
	};

	template<typename T>
	class TypedEvent {
		std::vector<std::function<void(void*, T)>> handlers;
	public:
		TypedEvent() : handlers() { }

		//TODO Retornar um objeto (Connections) para poder remover
		Connection operator+=(std::function<void(void*, T)> eventHandler) {
			handlers.push_back(eventHandler);
		}

		//TODO Remover a conexao
		void operator-=(const Connection& connection) {
		}

		void operator()(void* sender, T event) {
			for(auto& h : handlers) {
				h(sender, event);
			}
		}
	};

} // namespace engine

#endif /* TYPEDEVENT_H_ */
