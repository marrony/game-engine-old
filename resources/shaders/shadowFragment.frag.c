#version 120

uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

uniform vec3 myLightPosition;

uniform int mySplit;
uniform float myZNear;
uniform float myZFar;

//varying vec3 color;
//varying vec3 normal;
//varying vec3 tangent;
//varying vec3 bitangent;
varying vec4 position;

#define USE_STANDARD 0
#define USE_BIAS 0

void main(void) {
#if USE_STANDARD
	float zPosition = (-position.z - myZNear) / (myZFar - myZNear);
#else
	float zPosition = (-position.z - 0.01) / (200.0 - 0.01); //OK
//	float zPosition = (length(position.xyz));// - 0.01) / (200.0 - 0.01); //OK
#endif

#if USE_BIAS
	float ddx = dFdx(zPosition);
	float ddy = dFdy(zPosition);
	float bias = 0.25*(ddx*ddx + ddy*ddy);
#else
	float bias = 0.0;
#endif


	gl_FragData[0] = vec4(zPosition, zPosition*zPosition + bias, 0, 1);
	//gl_FragDepth = (-position.z - myZNear) / (myZFar - myZNear);
}
