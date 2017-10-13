#ifndef OPENAL_H
#define OPENAL_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"


// Init only on Android and HTML5
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
	#define SHOULD_INITIALIZE_OPENAL true
#else
	#define SHOULD_INITIALIZE_OPENAL false
#endif

class OpenAL {
private:
	OpenAL();
	~OpenAL();
	OpenAL(OpenAL const&);
	void operator=(OpenAL const&);
	static OpenAL* instance;
	bool is_initializable;
	bool is_initialized;
	bool is_suspended;
	ALCdevice* device;
	ALCcontext* context;
	std::vector<ALuint> sources, suspended_sources;
public:
	static OpenAL* getInstance();
	bool init();
	ALuint fromWav(char* name);
	ALuint newSource(unsigned char* data, ALsizei size, ALenum format, ALsizei frequency);
	const char* getSourceState(ALuint source);
	float getSourceTime(ALuint source);
	void setSourceTime(ALuint source, float seconds);
	void playSource(ALuint source);
	void pauseSource(ALuint source);
	void rewindSource(ALuint source);
	void stopSource(ALuint source);
	void removeSource(ALuint source);
	void suspend();
	void resume();
	void close();
};

#endif
