#include "LuaUtils.h"

void printStack(lua_State* L) {
	int n = lua_gettop(L);
	for (int i = 1; i <= n; i++)  {
		printf("STACK %d %s\r\n", i, lua_tostring(L, i));
	}
}

void checkArgCount(lua_State* L, int countExact) {
	int count = lua_gettop(L);
	if (count != countExact) {
		luaL_error(L, "This function requires %d argument(s). Got %d.", countExact, count);
	}
}

void checkArgCount(lua_State* L, int countFrom, int countTo) {
	int count = lua_gettop(L);
	if ((count < countFrom) || (count > countTo)) {
		luaL_error(L, "This function requires from %d to %d argument(s). Got %d.", countFrom, countTo, count);
	}
}

void checkString(lua_State* L, int index) {
	int luaType = lua_type(L, index);
	if (luaType != LUA_TSTRING) {
		luaL_error(L, "Wrong type. String expected at %d. Got %s.", index, lua_typename(L, luaType));
	}
}

void checkNumber(lua_State* L, int index) {
	int luaType = lua_type(L, index);
	if (luaType != LUA_TNUMBER) {
		luaL_error(L, "Wrong type. Number expected at %d. Got %s.", index, lua_typename(L, luaType));
	}
}

void checkBoolean(lua_State* L, int index) {
	int luaType = lua_type(L, index);
	if (luaType != LUA_TBOOLEAN) {
		luaL_error(L, "Wrong type. Boolean expected at %d. Got %s.", index, lua_typename(L, luaType));
	}
}

void checkTable(lua_State* L, int index) {
	int luaType = lua_type(L, index);
	if (luaType != LUA_TTABLE) {
		luaL_error(L, "Wrong type. Table expected at %d. Got %s.", index, lua_typename(L, luaType));
	}
}

void checkBuffer(lua_State* L, int index) {
	if (!dmScript::IsBuffer(L, index)) {
		luaL_error(L, "Wrong type. Buffer expected at %d. Got %s.", index, lua_typename(L, lua_type(L, index)));
	}
}
