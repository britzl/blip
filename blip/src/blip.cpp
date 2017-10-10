#include "sfxr.h"
#include <dmsdk/sdk.h>
#include "OpenAL.h"

#define LIB_NAME "Blip"
#define MODULE_NAME "blip"


static void play() {

}

static int Laser(lua_State* L) {
	play();
	return 0;
}

static const luaL_reg Module_methods[] = {
	{"laser", Laser},
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
