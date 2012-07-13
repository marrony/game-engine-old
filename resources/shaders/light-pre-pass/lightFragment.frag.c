#version 120

uniform mat4 myProjectionMatrix;
uniform mat4 myViewMatrixInverse;
uniform mat4 myViewMatrix;

uniform mat4 myLightViewMatrix;
uniform mat4 myLightProjectionMatrix;

uniform sampler2D myDepthMap;
uniform sampler2D myNormalMap;
uniform sampler2D myShadowMap;

uniform float myZNear;
uniform float myZFar;

uniform vec3 myLightPositionInCameraSpace;
uniform vec3 myLightDirectionInCameraSpace;
uniform vec3 myLightColor;
uniform vec3 myLightAttenuations;
uniform vec2 myPlanes;

uniform vec3 myCameraPosition;

uniform bool castShadow;

varying vec4 position;
varying vec3 viewDir;
varying vec2 texCoord;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

void main(void) {
	vec4 depth = texture2D(myDepthMap, texCoord);

	vec3 viewDirection = normalize(viewDir);

	vec3 pixelPositionInCameraSpace;
	pixelPositionInCameraSpace.z = - myPlanes.y / (myPlanes.x + depth.r);
	pixelPositionInCameraSpace.xy = (viewDirection.xy / viewDirection.z) * pixelPositionInCameraSpace.z;

	vec3 normal = normalize((texture2D(myNormalMap, texCoord).xyz * 2.0 - 1.0));

	float lightToPixelDist = length(myLightPositionInCameraSpace - pixelPositionInCameraSpace.xyz);

	vec3 lightToPixel = normalize(myLightPositionInCameraSpace - pixelPositionInCameraSpace.xyz);

	float lightAngle = dot(-lightToPixel, myLightDirectionInCameraSpace);

	if(lightAngle > cos(radians(50.0))) {
		float vsmFactor = 1.0;
		////////////////////////////////////////////////////////////////////////////////////
		if(castShadow) {
			vec4 pixelPositionInLightSpace = myLightViewMatrix * myViewMatrixInverse * vec4(pixelPositionInCameraSpace, 1.0);
			vec4 pixelPositionInLightClipSpace = myLightProjectionMatrix * pixelPositionInLightSpace;

			vec4 cameraPositionInLightSpace = myLightViewMatrix * vec4(myCameraPosition, 1);

	//		float lightToPixelDistance = (- pixelPositionInLightSpace.z - 0.01) / (myZFar - myZNear); //OK
			float lightToPixelDistance = (length(pixelPositionInLightSpace.xyz) - myZNear) / (myZFar - myZNear);

			vec2 shadowTexCoord = (pixelPositionInLightClipSpace.xy / pixelPositionInLightClipSpace.w) * 0.5 + 0.5;
			vec4 depthSample0 = texture2D(myShadowMap, shadowTexCoord);

			float M1 = depthSample0.x;
			float M2 = depthSample0.y;
			float N = 1.0;

			float mu = M1;
			float sigmatemp = M2/N - M1*M1;
			float sigma2 = max(sigmatemp, 0.00025);

			float diff = mu - lightToPixelDistance;
			float Pmax = sigma2 / (sigma2 + diff*diff);

			float isLit = lightToPixelDistance <= M1 ? 1.0 : 0.0;
			vsmFactor = max(isLit, Pmax);
		}

		////////////////////////////////////////////////////////////////////////////////////

		vec3 halfVector = normalize(viewDirection + lightToPixel); // half vector
		vec3 Idiff = vec3(saturate(dot(lightToPixel, normal))) * myLightColor;
		float Ispec = pow(saturate(dot(halfVector, normal)), 16.0);

		float att = saturate(1.0 - lightToPixelDist/myLightAttenuations.x);

		gl_FragData[0].rgb = vec3(0.05) + vsmFactor * att * Idiff;
		gl_FragData[0].a = vsmFactor * att * Ispec;
	} else {
		gl_FragData[0].rgba = vec4(0.05);
	}
}
