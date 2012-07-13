/*
 * SpatialLocation.cpp
 *
 *  Created on: 27/05/2011
 *      Author: marrony.neris
 */

#include "SpatialLocation.h"
#include "TransformationModifier.h"
#include "GameEntity.h"

namespace engine {

	SpatialLocation::SpatialLocation(TransformationModifier* modifier) :
		modifier(modifier), position(0, 0, 0) {
	}

	SpatialLocation::~SpatialLocation() {
	}

	math::Vector3 SpatialLocation::getUnmodifiedPosition() const {
		return position;
	}

	math::Vector3 SpatialLocation::getPosition() const {
		if(modifier != 0) {
			return modifier->getTransformation() * position;
		}

		return position;
	}

	void SpatialLocation::setPosition(const math::Vector3 & newPosition) {
		position = newPosition;
	}

	void SpatialLocation::updateBindings(GameEntity* owner) {
		modifier = owner->getComponent<TransformationModifier>();
	}

} /* namespace engine */
