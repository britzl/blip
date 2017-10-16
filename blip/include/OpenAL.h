#ifndef OPENAL_H
#define OPENAL_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <dmsdk/sdk.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"


// Init only on Android and HTML5
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
	#define SHOULD_INITIALIZE_OPENAL true
#else
	#define SHOULD_INITIALIZE_OPENAL false
#endif

struct OpenALSound {
	unsigned int source;
	unsigned int buffer;
};

class OpenAL {
private:
	OpenAL();
	~OpenAL();
	OpenAL(OpenAL const&);
	static OpenAL* instance;
	bool is_initializable;
	bool is_initialized;
	bool is_closed;
	ALCdevice* device;
	ALCcontext* context;
	dmArray<OpenALSound> sources;
public:
	static OpenAL* getInstance();
	bool init();
	ALuint fromWav(char* name);
	OpenALSound newSource(unsigned char* data, ALsizei size, ALenum format, ALsizei frequency);
	void play(OpenALSound sound);
	void stop(OpenALSound sound);
	void remove(OpenALSound sound);
	void close();
};

#endif
