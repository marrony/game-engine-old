/*
 * Transformation.h
 *
 *  Created on: 08/02/2011
 *      Author: marrony.neris
 */

#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace engine {

	namespace math {

		class Transformation {
			Vector3 position;
			Quaternion orientation;
			Vector3 scale;
		public:
			Transformation() :
					position(0, 0, 0), orientation(1, 0, 0, 0), scale(1, 1, 1) {
			}

			Quaternion getOrientation() const {
				return orientation;
			}

			Vector3 getPosition() const {
				return position;
			}

			Vector3 getScale() const {
				return scale;
			}

			void setOrientation(const Quaternion& newOrientation) {
				orientation = newOrientation;
			}

			void setPosition(const Vector3& newPosition) {
				position = newPosition;
			}

			void setScale(const Vector3& newScale) {
				scale = newScale;
			}

			void rotateBy(const Quaternion& delta) {
				orientation *= delta;
			}

			void translateBy(const Vector3& delta) {
				position += delta;
			}

			void scaleBy(const Vector3& delta) {
				scale *= delta;
			}

			Transformation concatenate(const Transformation& parent) const {
				Transformation result;

				result.orientation = parent.orientation * orientation;
				result.scale = parent.scale * scale;

				result.position = parent.orientation * (parent.scale * position);
				result.position += parent.position;

				return result;
			}

			Matrix4 createMatrix() const {
				return Matrix4::transformationMatrix(orientation, position, scale);
			}
		};

	} // namespace math

}// namespace engine

#endif /* TRANSFORMATION_H_ */
