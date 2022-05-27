#include <dmsdk/sdk.h>
#include <string.h>
#include "sfxr.h"

#define LIB_NAME "Blip"
#define MODULE_NAME "blip"


static void write32(unsigned char* buff, uint32_t offset, uint32_t value)
{
	buff[offset+0] = (value & 0x000000FF) >> 0;
	buff[offset+1] = (value & 0x0000FF00) >> 8;
	buff[offset+2] = (value & 0x00FF0000) >> 16;
	buff[offset+3] = (value & 0xFF000000) >> 24;
}
static void write16(unsigned char* buff, uint32_t offset, uint16_t value)
{
	buff[offset+0] = (value & 0x000000FF) >> 0;
	buff[offset+1] = (value & 0x0000FF00) >> 8;
}

// http://soundfile.sapp.org/doc/WaveFormat/
static unsigned char* SfxrToWav(SfxrSound sfxr_sound, uint32_t* sizeout)
{
	const uint32_t num_channels = 1;
	const uint32_t bits_per_sample = 16;
	const uint32_t bytes_per_sample = bits_per_sample / 8;
	const uint32_t sample_rate = 44100;
	const uint32_t byte_rate = sample_rate * num_channels * bytes_per_sample;
	const uint32_t block_align = num_channels * bytes_per_sample;
	const uint32_t num_samples = sfxr_sound.length / (num_channels * bytes_per_sample);
	const uint32_t subchunk1_size = 16;
	const uint32_t subchunk2_size = num_samples * num_channels * bytes_per_sample; // same as sfxr_sound.length
	const uint32_t chunk_size = 4 + (8 + subchunk1_size) + (8 + subchunk2_size);
	*sizeout = chunk_size;

	unsigned char* wav = (unsigned char*)malloc(chunk_size);

	wav[0]='R';wav[1]='I';wav[2]='F';wav[3]='F';			// ChunkID
	write32(wav, 4, chunk_size);							// ChunkSize
	wav[8]='W';wav[9]='A';wav[10]='V';wav[11]='E';			// Format
	wav[12]='f';wav[13]='m';wav[14]='t';wav[15]=' ';		// SubChunk1ID
	write32(wav, 16, subchunk1_size);						// Subchunk1Size - PCM
	write16(wav, 20, 1);									// AudioFormat - PCM
	write16(wav, 22, num_channels);							// NumChannels
	write32(wav, 24, sample_rate);							// SampleRate
	write32(wav, 28, byte_rate);							// ByteRate
	write16(wav, 32, block_align);							// BlockAlign
	write16(wav, 34, bits_per_sample);						// BitsPerSample
	wav[36]='d';wav[37]='a';wav[38]='t';wav[39]='a';		// SubChunk2ID
	write32(wav, 40, subchunk2_size);						// SubChunk2Size

	memcpy(wav + 44, sfxr_sound.data, sfxr_sound.length);
	return wav;
}

static int Create(SfxrSound (*f)(int), lua_State* L) {
	DM_LUA_STACK_CHECK(L, 1);
	float seed = luaL_checknumber(L, 1);
	SfxrSound sfxr_sound = (*f)(seed);

	uint32_t wav_size = 0;
	unsigned char* wav = SfxrToWav(sfxr_sound, &wav_size);
	if (wav == 0) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushlstring(L, (const char*)wav, wav_size);

	free(wav);
	free(sfxr_sound.data);
	return 1;
}

static int Blip(lua_State* L) {
	return Create(SfxrBlip, L);
}

static int Jump(lua_State* L) {
	return Create(SfxrJump, L);
}

static int Hurt(lua_State* L) {
	return Create(SfxrHurt, L);
}

static int Powerup(lua_State* L) {
	return Create(SfxrPowerup, L);
}

static int Explosion(lua_State* L) {
	return Create(SfxrExplosion, L);
}

static int Laser(lua_State* L) {
	return Create(SfxrLaser, L);
}

static int Pickup(lua_State* L) {
	return Create(SfxrPickup, L);
}


static const luaL_reg Module_methods[] = {
	{"blip", Blip},
	{"jump", Jump},
	{"hurt", Hurt},
	{"powerup", Powerup},
	{"explosion", Explosion},
	{"laser", Laser},
	{"pickup", Pickup},
	{0, 0}
};


static void LuaInit(lua_State* L) {
	int top = lua_gettop(L);
	luaL_register(L, MODULE_NAME, Module_methods);
	lua_pop(L, 1);

	assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeBlipExtension(dmExtension::AppParams* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeBlipExtension(dmExtension::Params* params) {
	LuaInit(params->m_L);
	return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeBlipExtension(dmExtension::AppParams* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeBlipExtension(dmExtension::Params* params) {
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(Blip, LIB_NAME, AppInitializeBlipExtension, AppFinalizeBlipExtension, InitializeBlipExtension, 0, 0, FinalizeBlipExtension)
