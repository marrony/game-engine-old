uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
//uniform mat3 myNormalMatrix;

uniform sampler2D myTextureId0;
uniform sampler2D myTextureId1;
uniform vec3 myDiffuseColor;
uniform vec3 mySpecularColor;

varying vec3 color;
varying vec3 normal;
varying vec3 tangent;
varying vec3 bitangent;

void main(void) {
	vec4 color0 = texture2D(myTextureId0, gl_TexCoord[0].st);
	vec4 color1 = texture2D(myTextureId1, gl_TexCoord[0].st);

	vec3 normal0 = normalize(normal);
	vec3 tangent0 = normalize(tangent);
	vec3 bitangent0 = normalize(bitangent);

	gl_FragData[0].rgb = normal0 * 0.5 + 0.5;
	gl_FragData[1].rgb = color * color0.rgb * color1.rgb * myDiffuseColor;
	gl_FragData[2].rgb = mySpecularColor;
}
