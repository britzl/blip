#include "OpenAL.h"
#include <dmsdk/sdk.h>
#ifdef __EMSCRIPTEN__
	#include "compatibility.h"
#endif

ALuint CreateBuffer(unsigned char* data, ALenum format, ALsizei size, ALsizei frequency) {
	ALuint buffer = 0;
	alGenBuffers(1, &buffer);
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		dmLogError("OpenAL Error: %s\n", alGetString(err));
	}

	//now we put our data into the openAL buffer and
	//check for success
	alBufferData(buffer, format, (void*)data, size, frequency);
	free(data);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR) {
		dmLogError("OpenAL Error: %s\n", alGetString(err));
		if (alIsBuffer(buffer)) {
			alDeleteBuffers(1, &buffer);
		}
		return 0;
	}

	return buffer;
}


OpenAL* OpenAL::instance = NULL;

OpenAL::OpenAL() :
	is_initializable(SHOULD_INITIALIZE_OPENAL),
	is_initialized(false) {
	sources.reserve(128);
}

OpenAL* OpenAL::getInstance() {
	if (!instance) {
		instance = new OpenAL();
	}
	return instance;
}

bool OpenAL::init() {
	if (!is_initialized && is_initializable) {
		device = alcOpenDevice(NULL);
		if (!device) {
			dmLogError("Failed to open audio device.\n");
			return 1;
		}
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
		if (!context) {
			dmLogError("Failed to make audio context.\n");
			return false;
		}
		is_initialized = true;
	}
	return true;
}

ALuint OpenAL::newSource(unsigned char* data, ALsizei size, ALenum format, ALsizei frequency) {
	// Load the sound into a buffer.
	ALuint buffer = CreateBuffer(data, AL_FORMAT_STEREO16, size, frequency);
	if (buffer == 0) {
		return 0;
	}

	// Create the source to play the sound with.
	ALuint source = 0;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);

	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		dmLogError("Error creating new source");
	}

	sources.push_back(source);
	return source;
}

void OpenAL::playSource(ALuint source) {
	alSourcePlay(source);
}

void OpenAL::stopSource(ALuint source) {
	alSourceStop(source);
}

void OpenAL::removeSource(ALuint source) {
	ALint buffer;
	alGetSourcei(source, AL_BUFFER, &buffer);
	alDeleteSources(1, &source);
	ALuint buffer_u = (ALuint)buffer;
	alDeleteBuffers(1, &buffer_u);

	std::vector<ALuint>::iterator it = std::find(sources.begin(), sources.end(), source);
	if (it != sources.end()) {
		std::swap(*it, sources.back());
		sources.pop_back();
	}
}

void OpenAL::close() {
	for (std::vector<ALuint>::iterator i = sources.begin(); i != sources.end(); ++i) {
		ALint buffer;
		alGetSourcei(*i, AL_BUFFER, &buffer);
		alDeleteSources(1, &*i);
		ALuint buffer_u = (ALuint)buffer;
		alDeleteBuffers(1, &buffer_u);
	}
	sources.clear();
	if (is_initialized) {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
}
