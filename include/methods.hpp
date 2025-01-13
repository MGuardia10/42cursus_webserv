#pragma once

#include <iostream>
#include "Client.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include <sys/stat.h>
#include <netinet/in.h>


void	get_method( std::string path, Client& client, HTTPRequest* request);
void	post_method( std::string path, Client& client, HTTPRequest* request);
void	delete_method( std::string path, Client& client, HTTPRequest* request);

std::pair<bool, bool>	check_mode( std::string path, struct stat *data, mode_t mode );
void	error_send( Client& client, int code, std::string page_path, HTTPRequest *request );