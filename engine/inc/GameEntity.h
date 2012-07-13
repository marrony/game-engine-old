/*
 * Entity.h
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#ifndef GAMEENTITY_H_
#define GAMEENTITY_H_

#include <vector>
#include <string>

namespace engine {

	class Component;

	class GameEntity {
		void notifyComponents();
	public:
		GameEntity(const std::string& newName);
		~GameEntity();

		void addComponent(Component* component, bool shoudDelete = true);
		void removeComponent(Component* component);

		template<typename Comp>
		void removeComponent() {
			removeComponent(getComponent<Comp>());
		}

		template<typename Comp>
		Comp* getComponent() const {
			for(size_t i = 0; i < components.size(); i++) {
				Comp* component = dynamic_cast<Comp*>(components[i]);

				if(component != 0) {
					return component;
				}
			}
			return 0;
		}

		std::string getName() const;

	private:
		typedef std::vector<Component*> ComponentList;

		ComponentList components;
		ComponentList componentsToDelete;
		std::string name;
	};

}  // namespace engine

#endif /* GAMEENTITY_H_ */
