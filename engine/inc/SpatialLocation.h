/*
 * SpatialLocation.h
 *
 *  Created on: 27/05/2011
 *      Author: marrony.neris
 */

#ifndef SPATIALLOCATION_H_
#define SPATIALLOCATION_H_

#include "Component.h"
#include "math/Vector.h"

namespace engine {

	class TransformationModifier;

	class SpatialLocation : public DataComponent {
		TransformationModifier* modifier;
		math::Vector3 position;
	public:
		SpatialLocation(TransformationModifier* modifier = 0);
		virtual ~SpatialLocation();

		math::Vector3 getUnmodifiedPosition() const;

		math::Vector3 getPosition() const;
		void setPosition(const math::Vector3& newPosition);

		virtual void updateBindings(GameEntity* owner);

		TransformationModifier* getModifier() const {
			return modifier;
		}
	};

} /* namespace engine */
#endif /* SPATIALLOCATION_H_ */
