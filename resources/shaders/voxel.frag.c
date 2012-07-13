uniform sampler2D depthSampler;
uniform sampler2D colorSampler;

uniform float pixelWidth;
uniform float pixelHeight;
uniform vec4 fillColor;
uniform float maxDim;
uniform int horRes, vertRes;

void main(void) {
	int radius = 3;
	int radiusTimes2 = radius * 2;

	float min = 1.0;
	float uStart = gl_TexCoord[0].x - float(radius) * pixelWidth;
	float u = uStart;
	float v = gl_TexCoord[0].y - float(radius) * pixelHeight;
	float uMin, vMin;

	vec4 color = fillColor;

	for(int i = 0; i <= radiusTimes2; ++i) {
		for(int j = 0; j <= radiusTimes2; ++j) {
			float depth = texture2D(depthSampler, vec2(u, v)).r;

			if(depth < min) {
				min = depth;
				uMin = u;
				vMin = v;
			}
			u += pixelWidth;
		}

		u = uStart;
		v += pixelHeight;
	}

	if(min != 1.0)
		color = texture2D(colorSampler, vec2(uMin, vMin));

	gl_FragColor = color;
}
