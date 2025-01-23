#pragma once

#include <iostream>
#include "./Client.hpp"
#include "./HTTPRequest.hpp"

struct ErrorPageData {
	int 		code;
	std::string path;
};

struct RequestData {
	ConfigBase::ReturnData		returnData;
	ErrorPageData				errorData;
	std::vector<std::string>	methods;
};

RequestData	validate_request( Client& client, HTTPRequest* request );