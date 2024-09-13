#ifndef _CLIENT_H
#define _CLIENT_H

#include <map>
#include <string>

std::string PostRequest(const char* endpoint, const std::map<std::string, std::string>& headers, const char* request);
int HttpRequest(const char* method, const char* endpoint, std::map<std::string, std::string> headers, const char* request, const char* cookie_file, std::string& response);

#endif
