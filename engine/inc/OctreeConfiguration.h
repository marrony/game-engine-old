/*
 * OctreeConfiguration.h
 *
 *  Created on: 25/03/2011
 *      Author: marrony.neris
 */

#ifndef OCTREECONFIGURATION_H_
#define OCTREECONFIGURATION_H_

namespace engine {

	class OctreeConfiguration {
	public:

		int maxLevel() const {
			return _maxLevel;
		}

		void maxLevel(int newValue) {
			_maxLevel = newValue;
		}

		int maxObjects() const {
			return _maxObjects;
		}

		void maxObjects(int newValue) {
			_maxObjects = newValue;
		}

		int minObjects() const {
			return _minObjects;
		}

		void minObjects(int newValue) {
			_minObjects = newValue;
		}
	private:
		int _maxLevel;
		int _maxObjects;
		int _minObjects;
	};

} // namespace engine

#endif /* OCTREECONFIGURATION_H_ */
