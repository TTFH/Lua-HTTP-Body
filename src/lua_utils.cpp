#include <stdexcept>

#include "lua_utils.h"

lua_State* LuaNewState() {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	return L;
}

void LuaClose(lua_State* L) {
	lua_close(L);
}

void LuaLoadFile(lua_State* L, const char* filename) {
	if (luaL_dofile(L, filename) != LUA_OK) {
		std::string error = lua_tostring(L, -1);
		lua_pop(L, 1);
		lua_close(L);
		throw std::runtime_error(error);
	}
}

void LuaLoadString(lua_State* L, std::string str) {
	if (luaL_loadstring(L, str.c_str()) != LUA_OK || lua_pcall(L, 0, 0, 0) != LUA_OK) {
		std::string error = lua_tostring(L, -1);
		lua_pop(L, 1);
		lua_close(L);
		throw std::invalid_argument(error);
	}
}

std::string LuaGetGlobalString(lua_State* L, const char* name) {
	std::string value;
	lua_getglobal(L, name);
	if (lua_isstring(L, -1))
		value = lua_tostring(L, -1);
	lua_pop(L, 1);
	return value;
}

bool LuaIsFunctionDefined(lua_State* L, const char* name) {
	lua_getglobal(L, name);
	bool is_function = lua_isfunction(L, -1);
	lua_pop(L, 1);
	return is_function;
}

std::string LuaGlobalTableToJson(lua_State* L, const char* name) {
	std::string json;
	lua_getglobal(L, name);
	if (lua_istable(L, -1)) {
		lua_getglobal(L, "json");
		lua_getfield(L, -1, "stringify");
		if (lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
				if (lua_isstring(L, -1))
					json = lua_tostring(L, -1);
			}
		}
		lua_pop(L, 3);
	}
	return json;
}

void LuaJsonToStackTable(lua_State* L, const std::string& json) {
	lua_getglobal(L, "json");
	lua_getfield(L, -1, "parse");
	if (!lua_isfunction(L, -1))
		throw std::runtime_error("json.parse is not a function.");
	lua_pushstring(L, json.c_str());
	if (lua_pcall(L, 1, 1, 0) != LUA_OK)
		throw std::runtime_error(lua_tostring(L, -1));
	lua_remove(L, -2); // Remove json.parse() from stack
}

std::string LuaRunFunctionStackTableToStr(lua_State* L, const char* name) {
	lua_getglobal(L, name);
	if (!lua_isfunction(L, -1))
		throw std::invalid_argument("Function is not defined.");
	if (!lua_istable(L, -2))
		throw std::runtime_error("Table is not defined.");
	lua_pushvalue(L, -2);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK)
		throw std::invalid_argument(lua_tostring(L, -1));
	std::string result;
	if (lua_isstring(L, -1))
		result = lua_tostring(L, -1);
	lua_pop(L, 2);
	return result;
}

std::string LuaSerializeStackTable(lua_State* L, const char* name) {
	std::string result = LuaRunFunctionStackTableToStr(L, "dump");
	result = std::string(name) + " = " + result;
	return result;
}
