uniform mat4 myProjectionMatrix;

attribute vec4 myVertex;
attribute vec2 myTexCoord;

void main(void) {
	gl_Position =  myProjectionMatrix * myVertex;
	gl_TexCoord[0].st = myTexCoord;
}
