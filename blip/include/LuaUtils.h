#include <dmsdk/sdk.h>

void printStack(lua_State* L);
void checkArgCount(lua_State* L, int countExact);
void checkArgCount(lua_State* L, int countFrom, int countTo);
void checkString(lua_State* L, int index);
void checkNumber(lua_State* L, int index);
void checkBoolean(lua_State* L, int index);
void checkTable(lua_State* L, int index);
void checkBuffer(lua_State* L, int index);
