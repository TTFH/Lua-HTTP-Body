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
	if (content_type != "application/lua" || content_type != "application/x-lua-table")
		return http::response(status::BAD_REQUEST, "Invalid Content-Type.");
	if (content_type == "application/json")
		return http::response(status::BAD_REQUEST, "Json? Who is Jason?");

	try {
		LuaState* L = LuaNewState();
		LuaLoadFile(L, "json_parser.lua");
		LuaLoadString(L, request_body);
		string url = LuaGetGlobalString(L, "url");
		string json_body = LuaGlobalTableToJson(L, "body");

		map<string, string> headers;
		headers["Authorization"] = http::getHeader(req, "Authorization");
		string api_response = PostRequest(url.c_str(), headers, json_body.c_str());

		LuaJsonToStackTable(L, api_response);
		string content = LuaRunFunctionStackTableToStr(L, "response", false);
		printf("%s\n", content.c_str());

		string lua_response = LuaRunFunctionStackTableToStr(L, "dump", true);
		lua_response = "response = " + lua_response;
		LuaClose(L);
		//return http::response(status::OK, content);
		return http::response(status::OK, lua_response);
	} catch (const invalid_argument& e) {
		return http::response(status::BAD_REQUEST, e.what());
	} catch (const runtime_error& e) {
		return http::response(status::INTERNAL_SERVER_ERROR, e.what());
	}
}

int main() {
	http::Server app;
	app.post("/lua-proxy", LuaToJsonProxy);
	app.listen(3000);
	return 0;
}
