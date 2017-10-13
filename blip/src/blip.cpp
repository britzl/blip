#include <dmsdk/sdk.h>
#include "OpenAL.h"
#include "LuaUtils.h"
#include "sfxr.h"

#define LIB_NAME "Blip"
#define MODULE_NAME "blip"

#ifndef DLIB_LOG_DOMAIN
#define DLIB_LOG_DOMAIN "blip"
#include <dmsdk/dlib/log.h>
#endif


static int Laser(lua_State* L) {
	checkNumber(L, 1);
	float seed = lua_tonumber(L, 1);

	SfxrSound sound = Laser(seed);
	ALuint source = OpenAL::getInstance()->newSource(sound.data, sound.length, AL_FORMAT_MONO16, 44100);

	lua_pushinteger(L, source);
	return 1;
}


static int Pickup(lua_State* L) {
	checkNumber(L, 1);
	float seed = lua_tonumber(L, 1);

	SfxrSound sound = Pickup(seed);
	ALuint source = OpenAL::getInstance()->newSource(sound.data, sound.length, AL_FORMAT_MONO16, 44100);

	lua_pushinteger(L, source);
	return 1;
}

static int Powerup(lua_State* L) {
	checkNumber(L, 1);
	float seed = lua_tonumber(L, 1);

	SfxrSound sound = Powerup(seed);
	ALuint source = OpenAL::getInstance()->newSource(sound.data, sound.length, AL_FORMAT_MONO16, 44100);

	lua_pushinteger(L, source);
	return 1;
}


static int Play(lua_State* L) {
	checkNumber(L, 1);
	ALuint source = lua_tonumber(L, 1);
	OpenAL::getInstance()->playSource(source);
	return 0;
}


static int Remove(lua_State* L) {
	checkNumber(L, 1);
	ALuint source = lua_tonumber(L, 1);
	dmLogError("remove sound %d", source);
	OpenAL::getInstance()->stopSource(source);
	OpenAL::getInstance()->removeSource(source);
	return 0;
}


static const luaL_reg Module_methods[] = {
	{"laser", Laser},
	{"pickup", Pickup},
	{"powerup", Powerup},
	{"play", Play},
	{"remove", Remove},
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
	bool result = OpenAL::getInstance()->init();
	return result ? dmExtension::RESULT_OK : dmExtension::RESULT_INIT_ERROR;
}

dmExtension::Result AppFinalizeBlipExtension(dmExtension::AppParams* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeBlipExtension(dmExtension::Params* params) {
	return dmExtension::RESULT_OK;
}

void OnEventBlipExtension(dmExtension::Params* params, const dmExtension::Event* event) {
}

dmExtension::Result UpdateBlipExtension(dmExtension::Params* params) {
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(Blip, LIB_NAME, AppInitializeBlipExtension, AppFinalizeBlipExtension, InitializeBlipExtension, UpdateBlipExtension, OnEventBlipExtension, FinalizeBlipExtension)
