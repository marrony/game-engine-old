/*
 * EntitySystem.h
 *
 *  Created on: 26/08/2012
 *      Author: marrony
 */

#ifndef ENTITYSYSTEM_H_
#define ENTITYSYSTEM_H_

#include <string>
#include "PackedArray.h"

namespace engine {

	typedef unsigned int EntityID;
	typedef unsigned int ComponentMask;
	typedef unsigned int ComponentID;

	class EntitySystem {
		struct Entity {
			std::string name;
			ComponentMask componentMask;
		};

		typedef PackedArray<Entity> EntityArray;

		EntityArray entities;
	public:
		EntityID createEntity(const std::string& name);
		bool destroyEntity(EntityID entityId);

		EntityID findByName(const std::string& name);

		ComponentMask getComponentMask(EntityID entityId);

		bool addComponent(EntityID entityId, ComponentID componentId);
		bool removeComponent(EntityID entityId, ComponentID componentId);
	};

} /* namespace engine */

#endif /* ENTITYSYSTEM_H_ */
