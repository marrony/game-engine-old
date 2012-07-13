/*
 * SceneManager.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <vector>
#include <string>
#include <map>

#include "Resource.h"

#include "Scene.h"

namespace engine {

	class SceneManager {
	public:
		SceneManager();
		~SceneManager();

		Scene* createScene() {
			return new Scene("");
		}
	};
}

#endif /* SCENEMANAGER_H_ */
