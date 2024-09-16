#include <map>
#include <string>
#include <stdio.h>
#include <stdexcept>

#include "src/client.h"
#include "src/server.h"
#include "src/lua_utils.h"

using namespace std;

http::response LuaToJsonProxy(const http::request& req) {
	string request_body = req.body;
	string content_type = http::getHeader(req, "Content-Type");

	if (request_body.length() == 0)
		return http::response(status::NO_CONTENT, "Empty request body.");
	if (content_type == "application/json")
		return http::response(status::UNSUPPORTED_MEDIA_TYPE, "Json? Who is Jason?");
	if (content_type != "application/lua" && content_type != "application/x-lua-table")
		return http::response(status::UNSUPPORTED_MEDIA_TYPE, "Invalid Content-Type.");

	try {
		LuaState* L = LuaNewState();
		LuaLoadFile(L, "json_parser.lua");
		LuaLoadString(L, request_body);
		string json_body = LuaGlobalTableToJson(L, "body");

		string url = http::getHeader(req, "Location");
		if (url.empty()) url = LuaGetGlobalString(L, "url");
		if (url.empty()) {
			// If no redirect, serialize body to JSON
			LuaClose(L);
			return http::response(status::CREATED, json_body);
		}

		// Execute request
		map<string, string> headers;
		headers["Authorization"] = http::getHeader(req, "Authorization");
		string api_response = PostRequest(url.c_str(), headers, json_body.c_str());
		LuaJsonToStackTable(L, api_response);

		string response;
		if (LuaIsFunctionDefined(L, "response")) {
			// Apply user-defined transform
			response = LuaRunFunctionStackTableToStr(L, "response");
		} else {
			// Serialize API response as Lua table
			response = LuaSerializeStackTable(L, "response");
		}
		LuaClose(L);
		return http::response(status::OK, response);
	} catch (const invalid_argument& e) {
		return http::response(status::BAD_REQUEST, e.what());
	} catch (const runtime_error& e) {
		return http::response(status::INTERNAL_SERVER_ERROR, e.what());
	}
}

http::response JsonToLua(const http::request& req) {
	string request_body = req.body;
	string content_type = http::getHeader(req, "Content-Type");

	if (request_body.length() == 0)
		request_body = "{}";
	if (content_type != "application/json")
		return http::response(status::UNSUPPORTED_MEDIA_TYPE, "Invalid Content-Type.");

	try {
		LuaState* L = LuaNewState();
		LuaLoadFile(L, "json_parser.lua");
		LuaJsonToStackTable(L, request_body);
		string lua_body = LuaSerializeStackTable(L, "body");
		LuaClose(L);
		return http::response(status::OK, lua_body);
	} catch (const invalid_argument& e) {
		return http::response(status::BAD_REQUEST, e.what());
	} catch (const runtime_error& e) {
		return http::response(status::INTERNAL_SERVER_ERROR, e.what());
	}
}

int main() {
	http::Server app;
	app.post("/lua-proxy", LuaToJsonProxy);
	app.post("/json-to-lua", JsonToLua);
	app.listen(3000);
	return 0;
}
