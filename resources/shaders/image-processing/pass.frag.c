// pass shsader

uniform sampler2D sampler0;
uniform vec2 tc_offset[9];

void main(void) {
	gl_FragColor = texture2D(sampler0, gl_TexCoord[0].st);
}
