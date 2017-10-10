#include "OpenAL.h"
#ifdef __EMSCRIPTEN__
	#include "compatibility.h"
#endif
/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */

struct RIFF_Header {
	char chunkID[4];
	unsigned int chunkSize; //size not including chunkSize or chunkID
	char format[4];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
	char subChunkID[4];
	unsigned int subChunkSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned int sampleRate;
	unsigned int byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
	char subChunkID[4]; //should contain the word data
	unsigned int subChunk2Size; //Stores the size of the data block
};

/*
 * Load wave file function. No need for ALUT with this
 */
static ALuint loadWavFile(dmBuffer::HBuffer* sourceBuffer) {
	uint8_t* sourcedata = 0;
	uint32_t datasize = 0;
	dmBuffer::GetBytes(*sourceBuffer, (void**)&sourcedata, &datasize);

	//Local Declarations
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	unsigned char* data;

	int cursor = 0;
	// Read in the first chunk into the struct
	int field_size = sizeof(riff_header.chunkID);
	memcpy(&riff_header.chunkID, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(riff_header.chunkSize);
	memcpy(&riff_header.chunkSize, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(riff_header.format);
	memcpy(&riff_header.format, &sourcedata[cursor], field_size);
	cursor += field_size;

	//check for RIFF and WAVE tag in memeory
	if ((strncmp(riff_header.chunkID, "RIFF", 4) != 0) && (strncmp(riff_header.chunkID, "WAVE", 4) != 0)) {
		dmLogError("Invalid WAV RIFF or WAVE header\n");
		return 0;
	}

	//Read in the 2nd chunk for the wave info
	field_size = sizeof(wave_format.subChunkID);
	memcpy(&wave_format.subChunkID, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.subChunkSize);
	memcpy(&wave_format.subChunkSize, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.audioFormat);
	memcpy(&wave_format.audioFormat, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.numChannels);
	memcpy(&wave_format.numChannels, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.sampleRate);
	memcpy(&wave_format.sampleRate, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.byteRate);
	memcpy(&wave_format.byteRate, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.blockAlign);
	memcpy(&wave_format.blockAlign, &sourcedata[cursor], field_size);
	cursor += field_size;

	field_size = sizeof(wave_format.bitsPerSample);
	memcpy(&wave_format.bitsPerSample, &sourcedata[cursor], field_size);
	cursor += field_size;

	//check for fmt tag in memory
	if (strncmp(wave_format.subChunkID, "fmt ", 4) != 0) {
		dmLogError("Invalid WAV fmt header.\n");
		return 0;
	}

	//check for extra parameters;
	if (wave_format.subChunkSize > 16) {
		cursor += sizeof(short);
	}

	// Skip all additional chunks until data chunk is found
	bool data_found = false;
	do {
		//Read in the the last byte of data before the sound file
		field_size = sizeof(wave_data.subChunkID);
		memcpy(&wave_data.subChunkID, &sourcedata[cursor], field_size);
		cursor += field_size;

		field_size = sizeof(wave_data.subChunk2Size);
		memcpy(&wave_data.subChunk2Size, &sourcedata[cursor], field_size);
		cursor += field_size;

		//check for data tag in memory
		if (strncmp(wave_data.subChunkID, "data", 4) == 0) {
			data_found = true;
		} else if (cursor >= datasize - sizeof(wave_data)) {
			dmLogError("Invalid WAV data header.\n");
			return 0;
		}
	} while (!data_found);

	//Allocate memory for data
	data = new unsigned char[wave_data.subChunk2Size];

	// Read in the sound data into the soundData variable
	memcpy(data, &sourcedata[cursor], wave_data.subChunk2Size);

	//Now we set the variables that we passed in with the
	//data from the structs
	ALsizei size = wave_data.subChunk2Size;
	ALsizei frequency = wave_format.sampleRate;
	ALenum format = AL_FORMAT_MONO8;
	//The format is worked out by looking at the number of
	//channels and the bits per sample.
	if (wave_format.numChannels == 1) {
		if (wave_format.bitsPerSample == 8) {
			format = AL_FORMAT_MONO8;
		} else if (wave_format.bitsPerSample == 16) {
			format = AL_FORMAT_MONO16;
		}
	} else if (wave_format.numChannels == 2) {
		if (wave_format.bitsPerSample == 8) {
			format = AL_FORMAT_STEREO8;
		} else if (wave_format.bitsPerSample == 16) {
			format = AL_FORMAT_STEREO16;
		}
	}
	//create our openAL buffer and check for success
	ALuint buffer = 0;
	alGenBuffers(1, &buffer);
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		dmLogError("OpenAL Error: %s\n", alGetString(err));
	}

	//now we put our data into the openAL buffer and
	//check for success
	alBufferData(buffer, format, (void*)data, size, frequency);
	delete data;

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
	is_initialized(false),
	is_suspended(false) {
	sources.reserve(128);
	suspended_sources.reserve(128);
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


ALuint OpenAL::newSource(dmBuffer::HBuffer* sourceBuffer) {
	// Load the sound into a buffer.
	ALuint buffer = loadWavFile(sourceBuffer);
	if (buffer == 0) {
		return 0;
	}

	// Create the source to play the sound with.
	ALuint source = 0;
	alGenSources(1, &source);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE); // Positioned audio by default.
	alSourcei(source, AL_BUFFER, buffer);
	sources.push_back(source);
	if (alGetError() == AL_NO_ERROR) {
		alSource3f(source, AL_POSITION, 0., 0., 0.);
		alSource3f(source, AL_VELOCITY, 0., 0., 0.);
		alSourcef(source, AL_REFERENCE_DISTANCE, 50.);
		alSourcef(source, AL_MAX_DISTANCE, 1000.);
	}
	return source;
}

void OpenAL::getSourceDefaults(ALuint source, float* gain, float* max_distance, float* rolloff_factor, float* reference_distance, float* min_gain, float* max_gain, float* cone_outer_gain, float* cone_inner_angle, float* cone_outer_angle, float* dx, float* dy, float* dz) {
	alGetSourcef(source, AL_GAIN, gain);
	alGetSourcef(source, AL_MAX_DISTANCE, max_distance);
	alGetSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);
	alGetSourcef(source, AL_REFERENCE_DISTANCE, reference_distance);
	#ifndef __EMSCRIPTEN__
		alGetSourcef(source, AL_MIN_GAIN, min_gain);
		alGetSourcef(source, AL_MAX_GAIN, max_gain);
	#else
		*min_gain = 0.f;
		*max_gain = 1.f;
	#endif
	alGetSourcef(source, AL_CONE_OUTER_GAIN, cone_outer_gain);
	alGetSourcef(source, AL_CONE_INNER_ANGLE, cone_inner_angle);
	alGetSourcef(source, AL_CONE_OUTER_ANGLE, cone_outer_angle);
	alGetSource3f(source, AL_DIRECTION, dx, dy, dz);
}

const char* OpenAL::getSourceState(ALuint source) {
	ALenum state = 0;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	switch (state) {
		case AL_INITIAL:
			return "initial";
		case AL_PLAYING:
			return "playing";
		case AL_PAUSED:
			return "paused";
		case AL_STOPPED:
			return "stopped";
	}
	return "";
}

float OpenAL::getSourceTime(ALuint source) {
	float offset;
	alGetSourcef(source, AL_SEC_OFFSET, &offset);
	return offset;
}

void OpenAL::setSourceTime(ALuint source, float seconds) {
	alSourcef(source, AL_SEC_OFFSET, seconds);
}

void OpenAL::playSource(ALuint source) {
	alSourcePlay(source);
}

void OpenAL::pauseSource(ALuint source) {
	alSourcePause(source);
}

void OpenAL::rewindSource(ALuint source) {
	alSourceRewind(source);
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

void OpenAL::suspend() {
	if (!is_suspended && is_initializable) {
		for (std::vector<ALuint>::iterator i = sources.begin(); i != sources.end(); ++i) {
			ALenum state = 0;
			alGetSourcei(*i, AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING) {
				alSourcePause(*i);
				suspended_sources.push_back(*i);
			}
		}
		is_suspended = true;
	}
}

void OpenAL::resume() {
	if (is_suspended) {
		for (std::vector<ALuint>::iterator i = suspended_sources.begin(); i != suspended_sources.end(); ++i) {
			alSourcePlay(*i);
		}
		suspended_sources.clear();
		is_suspended = false;
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
