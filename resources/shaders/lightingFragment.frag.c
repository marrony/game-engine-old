#version 120

uniform mat4 myViewMatrixInverse;

uniform mat4 myLightViewProjectionMatrix;
uniform mat4 myLightViewMatrix;
uniform mat4 myLightProjectionMatrix;

uniform sampler2D myNormalMap;
uniform sampler2D myDiffuseMap;
uniform sampler2D mySpecularMap;
uniform sampler2D myDepthMap;
uniform sampler2D myShadowMap;

uniform vec3 myCameraPosition;
uniform vec3 myCameraDirection;

uniform vec3 myLightColor;
uniform vec3 myLightPosition;
uniform vec3 myLightPosition2;
uniform vec3 myLightDirection;
uniform vec3 myLightAttenuation;

uniform vec2 myPlanes;

varying vec3 viewPosition;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

vec3 lighting(vec3 lightColor, vec3 lightPosition, vec3 lightAttenuation, vec3 lightSpotDirection,
		vec3 pixelPosition, vec3 viewDir, vec3 normal, vec3 Mdiff, vec3 Mspec, float Mshi) {

	vec3 lightToPixelDir = normalize(lightPosition - pixelPosition);
//	float lightRadius = length(lightToPixelDir);
//
//	lightToPixelDir = normalize(lightToPixelDir);

	vec3 pixelColor = vec3(0);

	float lightAngle = dot(-lightToPixelDir, lightSpotDirection);

	if(lightAngle > cos(radians(50.0))) {
//		vec4 pixelPositionInLightClipSpace = myLightViewProjectionMatrix * myViewMatrixInverse * vec4(pixelPosition, 1.0);

		vec4 pixelPositionInLightSpace = myLightViewMatrix * myViewMatrixInverse * vec4(pixelPosition, 1.0);
		vec4 pixelPositionInLightClipSpace = myLightProjectionMatrix * pixelPositionInLightSpace;

#define USE_STANDARD 0
#define USE_PCF 0

#if USE_STANDARD //Standard Shadow Maps
//		float lightToPixelDistance = (pixelPositionInLightClipSpace.z - 0.01) / (200.0 - 0.01);
		float lightToPixelDistance = (- pixelPositionInLightSpace.z - 0.01) / (200.0 - 0.01); //OK

		vec2 shadowTexCoord = (pixelPositionInLightClipSpace.xy / pixelPositionInLightClipSpace.w) * 0.5 + 0.5;
		float lightToOccluderDistance = texture2D(myShadowMap, shadowTexCoord).r + 0.018;

#if USE_PCF
		float dt = 1.0 / 256.0;
		lightToOccluderDistance += texture2D(myShadowMap, shadowTexCoord + vec2(+dt, 0.0)).r + 0.018;
		lightToOccluderDistance += texture2D(myShadowMap, shadowTexCoord + vec2(-dt, 0.0)).r + 0.018;
		lightToOccluderDistance += texture2D(myShadowMap, shadowTexCoord + vec2(0.0, +dt)).r + 0.018;
		lightToOccluderDistance += texture2D(myShadowMap, shadowTexCoord + vec2(0.0, -dt)).r + 0.018;

		lightToOccluderDistance /= 5.0;
#endif

		if(lightToOccluderDistance > lightToPixelDistance) {
			vec3 halfVector = normalize(viewDir + lightToPixelDir); // half vector

			float att = saturate(1.0 - (- pixelPositionInLightSpace.z)/90.0);

			vec3 Idiff = saturate(dot(lightToPixelDir, normal)) * Mdiff * lightColor;
			vec3 Ispec = pow(saturate(dot(halfVector, normal)), Mshi) * Mspec;

			pixelColor = att * (Idiff + Ispec);
		}
#else //Variance Shadow Maps
		float dt = 1.0 / 256.0;
		float lightToPixelDistance = (- pixelPositionInLightSpace.z - 0.01) / (200.0 - 0.01); //OK
//		float lightToPixelDistance = (length(pixelPositionInLightSpace.xyz));// - 0.01) / (200.0 - 0.01); //OK

		vec2 shadowTexCoord = (pixelPositionInLightClipSpace.xy / pixelPositionInLightClipSpace.w) * 0.5 + 0.5;
		vec4 depthSample0 = texture2D(myShadowMap, shadowTexCoord);

		vec3 halfVector = normalize(viewDir + lightToPixelDir); // half vector

		float M1 = depthSample0.x;
		float M2 = depthSample0.y;
		float N = 1.0;

		float mu = M1;
		float sigmatemp = M2/N - M1*M1;
		float sigma2 = max(sigmatemp, 0.0005);
//		float sigma2 = min(max(M2 - M1*M1, 0.0) + 0.0001, 1.0);

		float diff = mu - lightToPixelDistance;
		float Pmax = sigma2 / (sigma2 + diff*diff);

		float isLit = lightToPixelDistance <= M1 ? 1.0 : 0.0;
		float vsmFactor = max(isLit, Pmax);

		float att = saturate(1.0 - (- pixelPositionInLightSpace.z)/90.0);

		vec3 Idiff = saturate(dot(lightToPixelDir, normal)) * Mdiff * lightColor;
		vec3 Ispec = pow(saturate(dot(halfVector, normal)), Mshi) * Mspec;

		pixelColor = vsmFactor * att * (Idiff + Ispec);
#endif
	}

	return pixelColor;
}

void main(void) {
//	vec3 diffuse = texture2D(myDiffuseMap, gl_TexCoord[0].st).rgb;
	vec3 diffuse = vec3(1);
//	vec3 specular = texture2D(mySpecularMap, gl_TexCoord[0].st).rgb;
	vec3 specular = vec3(1);

	float depth = texture2D(myDepthMap, gl_TexCoord[0].xy).r;

	vec3 normal = (texture2D(myNormalMap, gl_TexCoord[0].st).xyz - 0.5) * 2.0;
	vec3 viewDir = normalize(viewPosition);

	vec3 position;
	position.z = - myPlanes.y / (myPlanes.x + depth);
	position.xy = (viewDir.xy / viewDir.z) * position.z;

//	vec3 position = viewDir * depth * 200.0;

	gl_FragData[0].rgb = lighting(myLightColor, myLightPosition, myLightAttenuation, myLightDirection,
									position, viewDir, normal, diffuse, specular, 50.0);
}
