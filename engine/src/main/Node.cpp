/*
 * Node.cpp
 *
 *  Created on: 30/01/2011
 *      Author: marrony
 */

#include "Node.h"

namespace engine {

	Node::Node() :
		parent(0),
		globalsNeedUpdate(true),
		transformationNeedUpdate(true) {
	}

	void Node::setParent(Node* _parent) {
		parent = _parent;
	}

	Node* Node::getParent() {
		return parent;
	}

	std::vector<Node*> Node::getChildren() {
		return children;
	}

	void Node::addChild(Node* child) {
		children.push_back(child);

		child->setParent(this);

		child->needUpdate();
	}

	void Node::needUpdate() {
		globalsNeedUpdate = true;
		transformationNeedUpdate = true;

		std::vector<Node*>::iterator ite;
		for(ite = children.begin(); ite != children.end(); ite++) {
			Node* node = *ite;
			node->needUpdate();
		}
	}

	void Node::updateTranformation() const {
		transformation = getOrUpdateGlobalTransformation().createMatrix();
		transformationNeedUpdate = false;
	}

	//TODO calcular as variaveis globais
	void Node::updateGlobals() const {
		if(parent == 0) {
			global = local;
		} else {
			global = local.concatenate(parent->getOrUpdateGlobalTransformation());
		}

		globalsNeedUpdate = false;
		transformationNeedUpdate = true;
	}

	const math::Transformation& Node::getOrUpdateGlobalTransformation() const {
		if(globalsNeedUpdate) {
			updateGlobals();
		}

		return global;
	}

	void Node::setPosition(const math::Vector3& newPosition) {
		local.setPosition(newPosition);
		needUpdate();
	}

	void Node::setOrientation(const math::Quaternion& newOrientation) {
		local.setOrientation(newOrientation);
		needUpdate();
	}

	void Node::setScale(const math::Vector3& newScale) {
		local.setScale(newScale);
		needUpdate();
	}

	math::Vector3 Node::getPosition() const {
		return local.getPosition();
	}

	math::Quaternion Node::getOrientation() const {
		return local.getOrientation();
	}

	math::Vector3 Node::getScale() const {
		return local.getScale();
	}

	math::Vector3 Node::getGlobalPosition() const {
		return getOrUpdateGlobalTransformation().getPosition();
	}

	math::Quaternion Node::getGlobalOrientation() const {
		return getOrUpdateGlobalTransformation().getOrientation();
	}

	math::Vector3 Node::getGlobalScale() const {
		return getOrUpdateGlobalTransformation().getScale();
	}

	void Node::translate(const math::Vector3& t) {
		local.translateBy(t);

		needUpdate();
	}

	void Node::rotate(const math::Quaternion& r) {
		local.rotateBy(r);

		needUpdate();
	}

	void Node::scale(const math::Vector3& s) {
		local.scaleBy(s);

		needUpdate();
	}

	math::Matrix4 Node::getTransformation() const {
		if(transformationNeedUpdate) {
			updateTranformation();
		}

		return transformation;
	}

} // namespace engine
