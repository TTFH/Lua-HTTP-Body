#ifndef _SERVER_H
#define _SERVER_H

#include "../lib/crow_all.h"

namespace http {
	typedef crow::request request;
	typedef crow::response response;

	class Server {
	private:
		crow::SimpleApp app;
	public:
		Server();
		void post(const std::string& path, std::function<response(const request&)> callback);
		void listen(int port);
	};

	std::string getHeader(const request& req, const std::string& header);
	std::string getAuthBearerToken(const request& req);
};

namespace status {
	const int OK = 200;
	const int CREATED = 201;
	const int NO_CONTENT = 204;
	const int BAD_REQUEST = 400;
	const int UNAUTHORIZED = 401;
	const int UNSUPPORTED_MEDIA_TYPE = 415;
	const int INTERNAL_SERVER_ERROR = 500;
};

using json = crow::json::wvalue;

#endif
