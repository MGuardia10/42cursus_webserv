#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "Server.hpp"

typedef void (*Function)(const std::string, ConfigBase&);

/* parse.cpp */
std::vector<Server>	parse( std::string const& filename );
void normalize_string(std::string& str);

/* parseServer.cpp */
void	parse_server( std::vector<Server> &servers, std::ifstream &file, std::string &line, std::map<std::string, Function> server_directives, std::map<std::string, Function> location_directives, bool &line_after_server );
bool	is_valid_directive( const std::string line );

/* parseLocation.cpp */
bool	is_location( const std::string line );
int	parse_location( std::ifstream &file, std::string &line, Server &server, std::map<std::string, Function> location_directives );

/* getDirectives.cpp */
std::map<std::string, Function> get_server_directives();
std::map<std::string, Function> get_location_directives();

/* parse.tpp */
template<typename T>
int add_directive( std::string const line, T &item, std::map<std::string, Function> const directives_map );

/* directives.cpp */
void add_address( const std::string line, ConfigBase &item );
void add_listen( const std::string line, ConfigBase &item );
void add_server_name( const std::string line, ConfigBase &item );
void add_root( const std::string line, ConfigBase &item );
void add_client_max_body_size( const std::string line, ConfigBase &item );
void add_error_page( const std::string line, ConfigBase &item );
void add_index( const std::string line, ConfigBase &item );
void add_autoindex( const std::string line, ConfigBase &item );
void add_cgi_pass( const std::string line, ConfigBase &item );
void add_return( const std::string line, ConfigBase &item );
void add_methods( const std::string line, ConfigBase &item );
void add_alias( const std::string line, ConfigBase &item );

#include "parse.tpp"
