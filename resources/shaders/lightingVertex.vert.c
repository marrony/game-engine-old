#version 120

uniform vec3 myLightColor;
uniform vec3 myLightPosition;
uniform vec3 myLightDirection;

attribute vec3 myVertex;
attribute vec2 myTexCoord;
attribute vec3 myViewVertex;

varying vec3 viewPosition;

void main(void) {
	viewPosition = -myViewVertex;
	gl_Position = vec4(myVertex, 1);
	gl_TexCoord[0].st = myTexCoord;
}
