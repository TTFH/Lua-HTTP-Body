#include "server.h"

namespace http {
	Server::Server() {
		app.loglevel(crow::LogLevel::Warning);
	}
	void Server::post(const std::string& path, std::function<response(const request&)> callback) {
		app.route_dynamic(path).methods(crow::HTTPMethod::Post)(callback);
	}
	void Server::listen(int port) {
		app.port(port).multithreaded().run();
	}

	std::string getHeader(const request& req, const std::string& header) {
		return req.get_header_value(header);
	}

	std::string getAuthBearerToken(const request& req) {
		const std::string BEARER_PREFIX = "Bearer ";
		std::string auth_header = getHeader(req, "Authorization");
		std::string bearer_token;
		if (auth_header.find(BEARER_PREFIX) == 0)
			bearer_token = auth_header.substr(BEARER_PREFIX.length());
		return bearer_token;
	}
}
