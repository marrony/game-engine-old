#version 120

uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;

uniform float myZNear;
uniform float myZFar;

varying vec4 position;

#define USE_STANDARD 0
#define USE_BIAS 0

void main(void) {
#if USE_STANDARD
	float zPosition = (-position.z - myZNear) / (myZFar - myZNear);
#else
//	float zPosition = (-position.z - myZNear) / (myZFar - myZNear); //OK
	float zPosition = (length(position.xyz) - myZNear) / (myZFar - myZNear);
#endif

#if USE_BIAS
	float ddx = dFdx(position.z);
	float ddy = dFdy(position.z);
	float bias = 0.25*(ddx*ddx + ddy*ddy);
#else
	float bias = 0.0;
#endif

	gl_FragData[0] = vec4(zPosition, zPosition*zPosition + bias, 0, 1);
	//gl_FragDepth = (-position.z - myZNear) / (myZFar - myZNear);
}
