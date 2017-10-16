#include "OpenAL.h"
#include <dmsdk/sdk.h>
#ifdef __EMSCRIPTEN__
	#include "compatibility.h"
#endif

ALenum CheckError(const char* message) {
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		dmLogError("OpenAL Error: %s (%s)\n", message, alGetString(err));
	}
	return err;
}

ALuint CreateBuffer(unsigned char* data, ALenum format, ALsizei size, ALsizei frequency) {
	ALuint buffer = 0;
	alGenBuffers(1, &buffer);
	CheckError("Generating buffer");

	alBufferData(buffer, format, (void*)data, size, frequency);
	free(data);

	if (CheckError("Copying to buffer")) {
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
	is_initialized(false),
	is_closed(false) {
	sources.SetCapacity(128);
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

OpenALSound OpenAL::newSource(unsigned char* data, ALsizei size, ALenum format, ALsizei frequency) {
	OpenALSound alSound = {0};

	// Load the sound into a buffer.
	ALuint buffer = CreateBuffer(data, AL_FORMAT_STEREO16, size, frequency);
	if (buffer == 0) {
		return alSound;
	}

	// Create the source to play the sound with.
	ALuint source = 0;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);
	CheckError("New source");

	alSound.buffer = buffer;
	alSound.source = source;
	sources.Push(alSound);
	return alSound;
}

void OpenAL::play(OpenALSound sound) {
	alSourcePlay(sound.source);
	CheckError("Playing source");
}

void OpenAL::stop(OpenALSound sound) {
	if(!is_closed) {
		alSourceStop(sound.source);
		CheckError("Stopping source");
	}
}

void OpenAL::remove(OpenALSound sound) {
	if(!is_closed) {
		alDeleteSources(1, &sound.source);
		CheckError("Deleting source");

		alDeleteBuffers(1, &sound.buffer);
		CheckError("Deleting buffer");

		for(int i = 0; i < sources.Size(); i++) {
			OpenALSound alSound = sources[i];
			if (alSound.buffer == sound.buffer && alSound.source == sound.source) {
				sources.EraseSwap(i);
				break;
			}
		}
	}
}

void OpenAL::close() {
	while(!sources.Empty()) {
		OpenALSound alSound = sources.Back();
		sources.Pop();
		alSourceStop(alSound.source);
		alDeleteSources(1, &alSound.source);
		alDeleteBuffers(1, &alSound.buffer);
	}
	if (is_initialized) {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
	is_closed = true;
}
