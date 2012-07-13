/*
 * Light.cpp
 *
 *  Created on: 21/01/2011
 *      Author: marrony.neris
 */

#include "Light.h"
#include "Node.h"
#include "Frustum.h"
#include "SpatialLocation.h"
#include "GameEntity.h"

namespace engine {

	Light::Light(LightType newType, Frustum* newFrustum) :
		frustum(newFrustum), type(newType), _castShadow(false), enabled(true) {
	}

	Light::~Light() {
		delete frustum;
	}

	LightType Light::getType() const {
		return type;
	}

	bool Light::castShadow() const {
		return _castShadow;
	}

	void Light::castShadow(bool cast) {
		_castShadow = cast;
	}

	Frustum* Light::getFrustum() const {
		return frustum;
	}

	math::Vector3 Light::getPosition() const {
		return frustum->getPosition();
	}

	void Light::setPosition(const math::Vector3& position) {
		frustum->setPosition(position);
	}

	math::Vector3 Light::getDirection() const {
		return frustum->getDirection();
	}

	void Light::setDirection(const math::Vector3& direction) {
		frustum->setDirection(direction);
	}

	math::Vector3 Light::getAttenuations() const {
		return attenuations;
	}

	void Light::setAttenuations(const math::Vector3& newAttenuations) {
		attenuations = newAttenuations;
	}

	/*colors*/
	math::Vector3 Light::getDiffuse() const {
		return diffuse;
	}

	void Light::setDiffuse(const math::Vector3& newDiffuse) {
		diffuse = newDiffuse;
	}

	math::Vector3 Light::getSpecular() const {
		return specular;
	}

	void Light::setSpecular(const math::Vector3& newSpecular) {
		specular = newSpecular;
	}

	math::Vector3 Light::getAmbient() const {
		return ambient;
	}

	void Light::setAmbient(const math::Vector3& newAmbient) {
		ambient = newAmbient;
	}

	math::Vector4 Light::scissorRect(float lightRange, const Frustum* myCamera, const int* viewport) const {

		engine::math::Vector4 scissor;
		engine::math::Vector3 corners[2];

		//Get the camera world position
		engine::math::Vector3 diagonal;

		//If the camera distance to the light source is lesser than
		//the light range, return the description of the whole screen
		engine::math::Vector3 dir = myCamera->getPosition() - getPosition();
		if(dir.length() - lightRange <= 0.001) {
			return engine::math::Vector4(viewport[0], viewport[1], viewport[2], viewport[3]);
		}

		/**************************************/
		/*                    + Upper corner  */
		/*                   /                */
		/*                  /                 */
		/*                 /                  */
		/*                /                   */
		/*               /                    */
		/*              /                     */
		/*            ( ) Light position      */
		/*            /                       */
		/*           /                        */
		/*          /                         */
		/*         /                          */
		/*        /                           */
		/*       /                            */
		/*      + Lower corner                */
		/**************************************/

		//Multiply the light range by square root of two since we will compute
		//the corners of square
		lightRange *= 1.41421356f;

		const engine::math::Matrix4& modelviewMatrix = myCamera->getViewMatrix();
		//const engine::math::Matrix4& projectionMatrix = myCamera->getProjectionMatrix();

		diagonal.x = modelviewMatrix.m00 + modelviewMatrix.m10;
		diagonal.y = modelviewMatrix.m01 + modelviewMatrix.m11;
		diagonal.z = modelviewMatrix.m02 + modelviewMatrix.m12;

		diagonal = diagonal * lightRange;

		//Compute the lower corner
		corners[0] = myCamera->project(getPosition() - diagonal, viewport);

		//Compute the upper corner
		corners[1] = myCamera->project(getPosition() + diagonal, viewport);

		//Set up the scissor info
		scissor.x = int(corners[0].x);
		scissor.y = int(corners[0].y);
		scissor.z = int(corners[1].x);
		scissor.w = int(corners[1].y);

		scissor.z -= scissor.x;
		scissor.w -= scissor.y;

		if(scissor.x < 0) {
			scissor.z += scissor.x;
			scissor.x = 0;
		}

		if(scissor.y < 0) {
			scissor.w += scissor.y;
			scissor.y = 0;
		}

		scissor.z = scissor.z > viewport[2] ? viewport[2] : scissor.z;
		scissor.w = scissor.w > viewport[3] ? viewport[3] : scissor.w;

		return scissor;
	}

	bool Light::isEnabled() const {
		return enabled;
	}

	void Light::enable() {
		enabled = true;
	}

	void Light::disable() {
		enabled = false;
	}

	bool Light::isVisible(const Frustum* const frustum) const {
//			return frustum == 0 || frustum->testAgainstFrustum(getFrustum()) != TestResult::OUT_SIDE;
		return frustum == 0 || getFrustum()->testAgainstFrustum(frustum) != TestResult::OUT_SIDE;
	}

}  // namespace engine
