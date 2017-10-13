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


static int Create(SfxrSound (*f)(int), lua_State* L) {
	checkNumber(L, 1);
	float seed = lua_tonumber(L, 1);

	SfxrSound sound = (*f)(seed);
	ALuint source = OpenAL::getInstance()->newSource(sound.data, sound.length, AL_FORMAT_MONO16, 44100);

	lua_pushinteger(L, source);
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

static int Play(lua_State* L) {
	checkNumber(L, 1);
	ALuint source = lua_tonumber(L, 1);
	OpenAL::getInstance()->playSource(source);
	return 0;
}


static int Remove(lua_State* L) {
	checkNumber(L, 1);
	ALuint source = lua_tonumber(L, 1);
	OpenAL::getInstance()->stopSource(source);
	OpenAL::getInstance()->removeSource(source);
	return 0;
}


static const luaL_reg Module_methods[] = {
	{"blip", Blip},
	{"jump", Jump},
	{"hurt", Hurt},
	{"powerup", Powerup},
	{"explosion", Explosion},
	{"laser", Laser},
	{"pickup", Pickup},
	{"play", Play},
	{"remove", Remove},
	{0, 0}
};

struct SfxrSound Blip(int seed);
struct SfxrSound Jump(int seed);
struct SfxrSound Hurt(int seed);
struct SfxrSound Powerup(int seed);
struct SfxrSound Explosion(int seed);
struct SfxrSound Laser(int seed);
struct SfxrSound Pickup(int seed);

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
