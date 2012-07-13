uniform mat4 myProjectionMatrix;
uniform mat4 myViewMatrixInverse;
uniform mat4 myViewMatrix;

attribute vec4 myVertex;
attribute vec2 myTexCoord;
attribute vec3 myViewVertex;

varying vec4 position;
varying vec3 viewDir;
varying vec2 texCoord;

void main(void) {
	position = myVertex;
	viewDir = -myViewVertex;
	texCoord = myTexCoord;

	gl_Position =  myProjectionMatrix * myVertex;
}
