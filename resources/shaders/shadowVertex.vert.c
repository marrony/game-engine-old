#version 120

uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
//uniform mat3 myNormalMatrix;

attribute vec4 myVertex;
//attribute vec3 myNormal;
//attribute vec3 myColor;
//attribute vec3 myTangent;
//attribute vec3 myBitantent;
//attribute vec2 myTexCoord;

//varying vec3 color;
//varying vec3 normal;
//varying vec3 tangent;
//varying vec3 bitangent;
varying vec4 position;

void main(void) {
//	color = myColor;
//	normal = myNormalMatrix * myNormal;
//	tangent = myNormalMatrix * myTangent;
//	bitangent = myNormalMatrix * myBitantent;
	position = myModelViewMatrix * myVertex;

	gl_Position =  myProjectionMatrix * myModelViewMatrix * myVertex;
}
