#ifndef _LUA_UTILS_H
#define _LUA_UTILS_H

#include <string>
#include "../lua5.1.4/lua.hpp"

#define LUA_OK 0
typedef lua_State LuaState;

lua_State* LuaNewState();
void LuaClose(lua_State* L);
void LuaLoadFile(lua_State* L, const char* filename);
void LuaLoadString(lua_State* L, std::string str);
std::string LuaGetGlobalString(lua_State* L, const char* name);
std::string LuaGlobalTableToJson(lua_State* L, const char* name);
void LuaJsonToStackTable(lua_State* L, const std::string& json);
std::string LuaRunFunctionStackTableToStr(lua_State* L, const char* name, bool pop);

#endif
