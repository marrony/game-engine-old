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

	template<typename... Args>
	class TypedEvent {
		std::vector<std::function<void(Args...)>> handlers;
	public:
		TypedEvent() : handlers() { }

		//TODO Retornar um objeto (Connections) para poder remover
		Connection operator+=(const std::function<void(Args...)>& eventHandler) {
			handlers.push_back(eventHandler);
		}

		//TODO Remover a conexao
		void operator-=(const Connection& connection) {
		}

		void operator()(Args&&... args) {
			for(auto& h : handlers) {
				h(std::forward<Args>(args)...);
			}
		}
	};

} // namespace engine

#endif /* TYPEDEVENT_H_ */
