/*
 * Node.h
 *
 *  Created on: 30/01/2011
 *      Author: marrony
 */

#ifndef NODE_H_
#define NODE_H_

#include "math/Quaternion.h"
#include "math/Transformation.h"
#include "Bounding.h"
#include "TransformationModifier.h"

#include <vector>

namespace engine {

	class Node : public TransformationModifier {
		Node* parent;
		std::vector<Node*> children;

		math::Transformation local;
		mutable math::Transformation global;

		mutable bool globalsNeedUpdate;
		mutable bool transformationNeedUpdate;

		mutable math::Matrix4 transformation;

		void needUpdate();
		void updateTranformation() const;
		void updateGlobals() const;

		const math::Transformation& getOrUpdateGlobalTransformation() const;
	public:
		Node();

		void setParent(Node* parent);
		Node* getParent();

		std::vector<Node*> getChildren();

		void addChild(Node* child);

		void setPosition(const math::Vector3& t);
		void setOrientation(const math::Quaternion& r);
		void setScale(const math::Vector3& s);

		math::Vector3 getPosition() const;
		math::Quaternion getOrientation() const;
		math::Vector3 getScale() const;

		math::Vector3 getGlobalPosition() const;
		math::Quaternion getGlobalOrientation() const;
		math::Vector3 getGlobalScale() const;

		void translate(const math::Vector3& t);
		void rotate(const math::Quaternion& r);
		void scale(const math::Vector3& s);

		virtual math::Matrix4 getTransformation() const;
	};

}  // namespace engine

#endif /* NODE_H_ */
