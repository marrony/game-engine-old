/*
 * Entity.cpp
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */


#include "GameEntity.h"
#include "Component.h"

#include <algorithm>

namespace engine {

	GameEntity::GameEntity(const std::string& newName) :
		name(newName) {
	}

	void GameEntity::notifyComponents() {
		for(size_t i = 0; i < components.size(); i++) {
			components[i]->updateBindings(this);
		}
	}

	void  GameEntity::removeComponent(Component* component) {
		ComponentList::iterator ite;

		ite = std::remove(components.begin(), components.end(), component);
		if(ite != components.end()) {
			components.erase(ite, components.end());
			notifyComponents();
			delete component;
		}

		ite = std::remove(componentsToDelete.begin(), componentsToDelete.end(), component);
		if(ite != componentsToDelete.end())
			componentsToDelete.erase(ite);
	}

	GameEntity::~GameEntity() {
		for(size_t i = 0; i < componentsToDelete.size(); i++) {
			delete componentsToDelete[i];
		}
	}

	void GameEntity::addComponent(Component* component, bool shoudDelete) {
		components.push_back(component);

		if(shoudDelete)
			componentsToDelete.push_back(component);
		
		notifyComponents();
	}

	std::string GameEntity::getName() const {
		return name;
	}

}  // namespace engine
