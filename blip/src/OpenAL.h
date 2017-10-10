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

#ifndef DLIB_LOG_DOMAIN
#define DLIB_LOG_DOMAIN "blip"
#include <dmsdk/dlib/log.h>
#endif

#include <dmsdk/sdk.h>

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
	ALuint newSource(dmBuffer::HBuffer* sourceBuffer);
	void getSourceDefaults(ALuint source, float* gain, float* max_distance, float* rolloff_factor, float* reference_distance, float* min_gain, float* max_gain, float* cone_outer_gain, float* cone_inner_angle, float* cone_outer_angle, float* dx, float* dy, float* dz);
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
