#version 120

uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;

uniform mat4 myBonePalete[32];

attribute vec4 myVertex;

attribute vec4 myBoneIds;
attribute vec4 myWeights;

varying vec4 position;

void main(void) {
	position = vec4(0);

	int boneId = int(myBoneIds.x);
	if(boneId >= 0) {
		position += (myBonePalete[boneId] * myVertex) * myWeights.x;
	}

	boneId = int(myBoneIds.y);
	if(boneId > 0) {
		position += (myBonePalete[boneId] * myVertex) * myWeights.y;
	}

	position = myModelViewMatrix * position;

	gl_Position =  myProjectionMatrix * position;
}
