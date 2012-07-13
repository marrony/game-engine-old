uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

uniform mat4 myBonePalete[32];

attribute vec4 myVertex;
attribute vec3 myNormal;
attribute vec3 myTangent;
attribute vec3 myBinormal;
attribute vec2 myTexCoord;

attribute vec4 myBoneIds;
attribute vec4 myWeights;

varying vec3 viewDirection;
varying vec4 position;
varying vec2 texCoord;
varying vec3 normal;
varying vec3 tangent;
varying vec3 binormal;

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
	viewDirection = -position.xyz;

	normal = myNormalMatrix * myNormal;
	tangent = myNormalMatrix * myTangent;
	binormal = myNormalMatrix * myBinormal;
	texCoord = myTexCoord;

	gl_Position =  myProjectionMatrix * position;

}
