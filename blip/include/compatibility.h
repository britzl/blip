// Add missing functions for HTML5 builds
void alGetSource3f(ALuint source, ALenum param, ALfloat *v1, ALfloat *v2, ALfloat *v3) {
	ALfloat values[3] = {0., 0., 0.};
	alGetSourcefv(source, param, values);
	*v1 = values[0];
	*v2 = values[1];
	*v3 = values[2];
}
