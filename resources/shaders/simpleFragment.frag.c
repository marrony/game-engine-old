uniform sampler2D myTexture0;
uniform sampler2D myTexture1;

uniform vec3 myLightColor;
uniform vec3 myLightPosition;

varying vec3 viewDirection;
varying vec4 position;
varying vec2 texCoord;
varying vec3 normal;
varying vec3 tangent;
varying vec3 binormal;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

vec3 lighting(vec3 lightColor, vec3 lightPosition, vec3 lightAttenuation, vec3 position,
		vec3 viewDir, vec3 normal, vec3 Mdiff, vec3 Mspec, float Mshi) {

	vec3 lightDir = lightPosition - position;
	vec3 halfVector = normalize(viewDir + lightDir); // half vector

	float len = length(lightDir);
	float att = saturate(1.0 - len/1000.0);

	lightDir /= len;

	vec3 Idiff = max(dot(lightDir, normal), 0.0) * Mdiff * lightColor;
	vec3 Ispec = pow(max(dot(halfVector, normal), 0.0), Mshi) * Mspec;

	return att * (Idiff + Ispec);
}

void main(void) {
   vec4 diffuse = texture2D(myTexture0, texCoord.st);
   vec3 n = texture2D(myTexture1, texCoord.st).xyz;

   vec3 n2 = normalize(tangent*n.x + binormal*n.y + normal*n.z);

   vec3 viewDir = normalize(viewDirection);

   gl_FragColor.rgb = lighting(myLightColor, myLightPosition, vec3(1, 1, 1), position.xyz,
   		viewDir, n2, diffuse.xyz, vec3(1, 1, 1), 128.0);

   gl_FragColor.a = 1.0;
}
