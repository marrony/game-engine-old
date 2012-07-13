/**
 * cgc -oglsl -strict -glslWerror -profile gpu_fp materialFragment.frag.c > frag.txt
 */

#version 120

uniform mat4 myModelViewMatrix;
uniform mat4 myProjectionMatrix;
uniform mat3 myNormalMatrix;

uniform vec2 myBufferSize;
uniform vec2 myScreenSize;

uniform float myZNear;
uniform float myZFar;

uniform float myObjectId;

uniform sampler2D lightBuffer;
uniform sampler2D objectIdBuffer;
uniform sampler2D myTexture0;
uniform sampler2D myTexture1;

varying vec4 position;
varying vec4 positionInScreenSpace;
varying vec3 normal;
varying vec2 texCoord;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

vec4 lerp(vec4 a, vec4 b, float alpha) {
	return alpha*b + (1.0-alpha)*a;
}

bool fuzzyEquals(float a, float b, float epsilon) {
	return abs(a - b) < epsilon;
}

vec4 sampleDSF(sampler2D samp, sampler2D depthIDSamp, vec2 texCoord,
				 vec2 texSize, float instanceID, float depthVS,
				 vec2 texScale, vec2 offset, vec2 filterScale)
{
	// Scale out into pixel space
	vec2 unormSamplePos = texSize * texCoord;

	// Determine the lerp amounts
	vec2 lerps = (fract(unormSamplePos) - (0.5f * texScale) - offset) / filterScale ;
	float lerpAmount[3];
	lerpAmount[0] = lerps.x;
	lerpAmount[1] = lerps.x;
	lerpAmount[2] = lerps.y;

	// Get the upper left position
	vec2 lerpPos = floor(unormSamplePos) + offset + 0.5f;
	lerpPos /= texSize;

	// Figure out our 4 sample points
	vec2 samplePoints[4];
	samplePoints[0] = lerpPos;
	samplePoints[1] = lerpPos + vec2(filterScale.x / texSize.x, 0);
	samplePoints[2] = lerpPos + vec2(0, filterScale.y / texSize.y);
	samplePoints[3] = lerpPos + vec2(filterScale.x / texSize.x, filterScale.y / texSize.y);

	/**
	 *    2                   3
	 *    +-------------------+
	 *    |                   |
	 *    |                   |
	 *    |                   |
	 *    |                   |
	 *    |                   |
	 *    |                   |
	 *    |                   |
	 *    +-------------------+
	 *    0                   1
	 */

	// Take the 4 samples, and compute an additional weight for
	// each sample based on comparison with the DepthID buffer
	vec4 samples[4];
	float weights[4];
	vec2 depthID;

#if 0
	for(int i = 0; i < 4; i++) {
		depthID = texture2D(depthIDSamp, samplePoints[i]).xy;
		weights[i] = fuzzyEquals(depthID.x, instanceID, 0.01f) && fuzzyEquals(depthID.y, depthVS, 0.1f) ? 1.0 : 0.0;
		samples[i] = texture2D(samp, samplePoints[i]);
	}
#else
	depthID = texture2D(depthIDSamp, samplePoints[0]).xy;
	weights[0] = fuzzyEquals(depthID.x, instanceID, 0.01f) && fuzzyEquals(depthID.y, depthVS, 0.1f) ? 1.0 : 0.0;
	samples[0] = texture2D(samp, samplePoints[0]);

	depthID = texture2D(depthIDSamp, samplePoints[1]).xy;
	weights[1] = fuzzyEquals(depthID.x, instanceID, 0.01f) && fuzzyEquals(depthID.y, depthVS, 0.1f) ? 1.0 : 0.0;
	samples[1] = texture2D(samp, samplePoints[1]);

	depthID = texture2D(depthIDSamp, samplePoints[2]).xy;
	weights[2] = fuzzyEquals(depthID.x, instanceID, 0.01f) && fuzzyEquals(depthID.y, depthVS, 0.1f) ? 1.0 : 0.0;
	samples[2] = texture2D(samp, samplePoints[2]);

	depthID = texture2D(depthIDSamp, samplePoints[3]).xy;
	weights[3] = fuzzyEquals(depthID.x, instanceID, 0.01f) && fuzzyEquals(depthID.y, depthVS, 0.1f) ? 1.0 : 0.0;
	samples[3] = texture2D(samp, samplePoints[3]);
#endif

	// We'll bias our lerp weights based on our additional DepthID
	// weights.  This will filter out "bad" samples that are across
	// discontinuities .
	lerpAmount[0] = saturate(lerpAmount[0] - weights[0] + weights[1]);
	lerpAmount[1] = saturate(lerpAmount[1] - weights[2] + weights[3]);

	float topWeight = (weights[0] + weights[1]) * 0.5f;
	float bottomWeight = (weights[2] + weights[3]) * 0.5f;
	lerpAmount[2] = saturate(lerpAmount[2] - topWeight + bottomWeight);

	// Perform the bilinear filtering with our new weights
	vec4 a = lerp(samples[0], samples[1], lerpAmount[0]);
	vec4 b = lerp(samples[2], samples[3], lerpAmount[1]);
	return lerp(a, b, lerpAmount[2]);
}

void main(void) {
	float depth = - position.z / myZFar;
	vec2 texBuffer = (positionInScreenSpace.xy / positionInScreenSpace.w) * 0.5 + 0.5;

	float id = myObjectId;

	vec2 texScale = myBufferSize / myScreenSize;
	vec4 light = sampleDSF(lightBuffer, objectIdBuffer, texBuffer, myBufferSize, id, depth, texScale, vec2(0), vec2(1));

	vec4 color0 = texture2D(myTexture0, texCoord);
	vec4 color1 = vec4(1);//texture2D(myTexture1, texCoord);

	vec3 pixelColor = color0.rgb * color1.rgb * (light.rgb + vec3(light.a));
//	vec3 pixelColor = (light.rgb + vec3(light.a));

	gl_FragColor = vec4(pixelColor, 1.0);
}
