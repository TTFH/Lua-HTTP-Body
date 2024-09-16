#include <string.h>
#include <stdexcept>

#define CURL_STATICLIB
#include <curl/curl.h>

#include "client.h"

size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* response) {
	response->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int HttpRequest(const char* method, const char* endpoint, std::map<std::string, std::string> headers, const char* request, const char* cookie_file, std::string& response) {
	int http_code = 500;
	response.clear();
	CURL* curl = curl_easy_init();
	if (curl != nullptr) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
		curl_easy_setopt(curl, CURLOPT_URL, endpoint);
		if (request != nullptr && strlen(request) > 0)
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

		curl_slist* curl_headers = nullptr;
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
			std::string header = it->first + ": " + it->second;
			curl_headers = curl_slist_append(curl_headers, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);

		curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-bundle.crt"); // C:\msys64\usr\ssl\certs
		if (cookie_file != nullptr && strlen(cookie_file) > 0) {
			curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie_file);
			curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie_file);
		}

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		else
			response = curl_easy_strerror(res);

		curl_slist_free_all(curl_headers);
		curl_easy_cleanup(curl);
	} else
		response = "Failed to initialize Curl.";
	return http_code;
}

std::string PostRequest(const char* endpoint, const std::map<std::string, std::string>& headers, const char* request) {
	std::string response;
	std::map<std::string, std::string> headers_copy(headers);
	headers_copy["Content-Type"] = "application/json";
	int status = HttpRequest("POST", endpoint, headers_copy, request, nullptr, response);
	if (status != 200) throw std::invalid_argument(response);
	return response;
}
