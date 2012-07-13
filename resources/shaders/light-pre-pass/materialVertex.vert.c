uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

uniform mat4 myBonePalete[32];

attribute vec4 myVertex;
attribute vec3 myNormal;
attribute vec2 myTexCoord;

attribute vec4 myBoneIds;
attribute vec4 myWeights;

varying vec4 position;
varying vec4 positionInScreenSpace;
varying vec3 normal;
varying vec2 texCoord;

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

	normal = myNormalMatrix * myNormal;
	texCoord = myTexCoord;
	positionInScreenSpace = myProjectionMatrix * position;
	gl_Position = positionInScreenSpace;
}
