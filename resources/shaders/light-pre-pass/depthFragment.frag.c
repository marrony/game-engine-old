uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

uniform float myObjectId;

uniform float myZNear;
uniform float myZFar;

uniform sampler2D myTexture0;
uniform sampler2D myTexture1;

varying vec4 position;
varying vec2 texCoord;
varying vec3 normal;
varying vec3 tangent;
varying vec3 binormal;

void main(void) {
	float zPosition = - position.z / myZFar;
	float objectId = myObjectId;

	vec3 n = normalize(texture2D(myTexture1, texCoord).rgb * 2.0 - 1.0);

	vec3 n2 = normalize(tangent*n.x + binormal*n.y + normal*n.z);
//	vec3 n2 = normalize(normal);

	gl_FragData[0].rgb = n2 * 0.5 + 0.5;
	gl_FragData[1] = vec4(objectId, zPosition, 1, 1);

//	gl_FragDepth = zPosition;
//	gl_FragDepth = gl_FragCoord.z;
}
