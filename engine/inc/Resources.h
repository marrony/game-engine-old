/*
 * Resources.h
 *
 *  Created on: 01/08/2012
 *      Author: marrony
 */

#ifndef RESOURCES_H_
#define RESOURCES_H_

namespace engine {

	template<typename T>
	struct Resources {
		std::vector<T> resources;
		std::vector<int> free;

		int add(const T& resource) {
			if(!free.empty()) {
				int handle = free.back();
				free.pop_back();

				resources[handle] = resource;
				return handle + 1;
			}

			resources.push_back(resource);
			return resources.size();
		}

		void remove(int handle) {
			if(handle == 0) return;
			free.push_back(--handle);
		}

		T& get(int handle) {
			return resources[--handle];
		}
	};

}  // namespace engine

#endif /* RESOURCES_H_ */
