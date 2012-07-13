uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

attribute vec4 myVertex;

varying vec4 position;

void main(void) {
	position = myModelViewMatrix * myVertex;

	gl_Position =  myProjectionMatrix * myModelViewMatrix * myVertex;
}
