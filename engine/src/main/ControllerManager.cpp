/*
 * ControllerManager.cpp
 *
 *  Created on: 04/03/2011
 *      Author: marrony.neris
 */

#include "ControllerManager.h"
#include "Component.h"
#include <algorithm>

namespace engine {

	ControllerManager::~ControllerManager() {
		for(size_t i = 0; i < controllers.size(); i++) {
			delete controllers[i];
		}
		controllers.clear();
	}

	void ControllerManager::addController(ControllerComponent* controller) {
		controllers.push_back(controller);
	}

	void ControllerManager::update(const GameTime& gameTime) {
		std::for_each(controllers.begin(), controllers.end(), [&](ControllerComponent* controller) {
			controller->update(gameTime);
		});
	}

}  // namespace engine
