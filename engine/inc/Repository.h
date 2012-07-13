/*
 * Repository.h
 *
 *  Created on: 05/05/2011
 *      Author: marrony
 */

#ifndef REPOSITORY_H_
#define REPOSITORY_H_

#include "Component.h"
#include "GameEntity.h"

namespace engine {

	template<typename T>
	class Repository : public DataComponent {
	public:
		virtual ~Repository() {
			for(size_t i = 0; i < components.size(); i++) {
				delete components[i];
			}

			components.clear();
		}

		void addComponent(T* component) {
			components.push_back(component);
		}

		std::vector<T*> getComponents() {
			return components;
		}

	private:
		std::vector<T*> components;
	};

	class RepositoryManager {
	public:
		RepositoryManager() :
			repository("repositories") {
		}

		template<typename S>
		void addRepository(Repository<S>* newRepository) {
			repository.addComponent(newRepository);
		}

		template<typename S>
		Repository<S>* getRepositoryForComponent() {
			return repository.getComponent<Repository<S> >();
		}

		template<typename T>
		T* getRepository() {
			return repository.getComponent<T>();
		}

	private:
		Entity repository;
	};


}  // namespace engine

#endif /* REPOSITORY_H_ */
