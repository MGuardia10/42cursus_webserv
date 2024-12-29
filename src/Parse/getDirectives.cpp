#include "../../include/parse.hpp"

std::map<std::string, Function> get_server_directives() {

	std::map<std::string, Function> serverMethods;

	serverMethods["address "] = add_address;
	serverMethods["listen "] = add_listen;
	serverMethods["server_name "] = add_server_name;
	serverMethods["root "] = add_root;
	serverMethods["client_max_body_size "] = add_client_max_body_size;
	serverMethods["error_page "] = add_error_page;
	serverMethods["index "] = add_index;
	serverMethods["autoindex "] = add_autoindex;
	serverMethods["cgi_pass "] = add_cgi_pass;
	serverMethods["return "] = add_return;
	serverMethods["methods "] = add_methods;

	return serverMethods;
}

std::map<std::string, Function> get_location_directives() {

	std::map<std::string, Function> locationMethods;

	locationMethods["alias "] = add_alias;
	locationMethods["root "] = add_root;
	locationMethods["client_max_body_size "] = add_client_max_body_size;
	locationMethods["error_page "] = add_error_page;
	locationMethods["index "] = add_index;
	locationMethods["autoindex "] = add_autoindex;
	locationMethods["cgi_pass "] = add_cgi_pass;
	locationMethods["return "] = add_return;
	locationMethods["methods "] = add_methods;

	return locationMethods;
}
