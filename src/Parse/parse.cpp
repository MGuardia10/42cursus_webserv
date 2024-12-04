#include "../../include/parse.hpp"

/* FIXME: servers harcoded so the implementations can be done while the parse is not completed */ 
static std::vector<Server> hardcodedServers( void )
{
	std::vector<Server>	servers;

	/* TODO: Server 1 */
	Server s1;

	s1.set_ip("127.0.0.1");
	s1.set_port(8080);
	s1.set_server_name("Test");

	s1.add_error_page(400, "/400.html");
	s1.add_error_page(404, "/404.html");

	s1.set_client_max_size(1000);

	s1.add_index("index.html");
	s1.set_autoindex(false);

	s1.add_cgi("sh", "/bin/bash");
	s1.add_cgi("py", "/bin/python3");

	ConfigBase::ReturnData s1_return = {.code = 300, .text = "Redirigiendo..."};
	s1.set_return(s1_return);

	s1.add_method("GET");
	s1.add_method("DELETE");
	s1.add_method("POST");
	s1.add_method("DELETE");

	Location l1s1 = Location("/");
	ConfigBase::ReturnData l1s1_return = {.code = 200, .text = "Acceso a location /"};
	l1s1.set_return(l1s1_return);
	l1s1.add_method("DELETE");
	s1.add_location(l1s1.get_route(), l1s1);

	Location l2s1 = Location("/test");
	l2s1.add_cgi("py", "/bin/python");
	l2s1.set_alias("port.html");
	l2s1.set_client_max_size(123);
	s1.add_location(l2s1.get_route(), l2s1);

	servers.push_back(s1);

	/* TODO: Server 2*/
	Server s2;

	s2.set_ip("127.0.0.1");
	s2.set_port(8081);
	s2.set_server_name("Test2");
	s2.set_root("/");

	s2.set_client_max_size(1000);

	Location l1s2 = Location("/");
	l1s2.set_root("/pages");
	l1s2.add_index("index.html");
	s2.add_location(l1s2.get_route(), l1s2);

	Location l2s2 = Location("/redirect");
	ConfigBase::ReturnData l2s2_return = {
		.code = 300,
		.text = "https://www.google.com"
	};
	l2s2.set_return(l2s2_return);
	l2s2.add_method("DELETE");
	l2s2.add_method("POST");
	s2.add_location(l2s2.get_route(), l2s2);

	servers.push_back(s2);

	return servers;
}

std::vector<Server>	parse( std::string const& filename )
{
	(void) filename;
	return hardcodedServers();
}
