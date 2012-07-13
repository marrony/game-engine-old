// blur (low-pass) 3x3 kernel

uniform sampler2D sampler0;
uniform vec2 tc_offset[9];

void main(void) {
	float TextelScale = 1.0 / 256.0;

	vec4 value = vec4(0);

	float Coefficients[21] = {
			0.000272337, 0.000892960, 0.002583865,
			0.006598130, 0.014869116, 0.029570767,
			0.051898313, 0.080381679, 0.109868729,
			0.132526984, 0.141074240, 0.132526984,
			0.109868729, 0.080381679, 0.051898313,
			0.029570767, 0.014869116, 0.006598130,
			0.002583865, 0.000892960, 0.000272337
	};

	for(int Index = 0; Index < 21; Index++) {
		vec2 texCoord;

		texCoord.x = gl_TexCoord[0].x;
		texCoord.y = gl_TexCoord[0].y + (Index - 10) * TextelScale;

		value += texture2D(sampler0, texCoord) * Coefficients[Index];
	}


	gl_FragColor = value;
}
