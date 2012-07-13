uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

varying vec4 position;

void main(void) {
//	float zPosition = - position.z / 200.0;
//	gl_FragDepth = zPosition;

	gl_FragDepth = gl_FragCoord.z;
}
