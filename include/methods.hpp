#pragma once

#include <iostream>
#include "Client.hpp"
#include "HTTPRequest.hpp"

void	get_method( std::string path, Client& client, HTTPRequest* request);
void	post_method( std::string path, Client& client, HTTPRequest* request);
void	delete_method( std::string path, Client& client, HTTPRequest* request);