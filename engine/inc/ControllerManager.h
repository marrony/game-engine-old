/*
 * ControllerManager.h
 *
 *  Created on: 04/03/2011
 *      Author: marrony.neris
 */

#ifndef CONTROLLERMANAGER_H_
#define CONTROLLERMANAGER_H_

#include <vector>

namespace engine {

	class ControllerComponent;
	class GameTime;

	class ControllerManager {
	public:
		~ControllerManager();

		void addController(ControllerComponent* controller);
		void update(const GameTime& gameTime);
	private:
		std::vector<ControllerComponent*> controllers;
	};

}

#endif /* CONTROLLERMANAGER_H_ */
