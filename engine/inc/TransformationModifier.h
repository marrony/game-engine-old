/*
 * Hierarchical.h
 *
 *  Created on: 27/05/2011
 *      Author: marrony.neris
 */

#ifndef TRANSFORMATIONMODIFIER_H_
#define TRANSFORMATIONMODIFIER_H_

#include "Component.h"
#include "math/Matrix.h"

namespace engine {

	class TransformationModifier : public DataComponent {
	public:
		virtual ~TransformationModifier() {};

		virtual math::Matrix4 getTransformation() const = 0;
	};

} /* namespace engine */
#endif /* TRANSFORMATIONMODIFIER_H_ */
