#include "../../include/parse.hpp"

void add_address( std::string line, ConfigBase &item ) {
	
	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	if ( line.compare("localhost") == 0 )
		server.set_ip( IP_DEFAULT );
	else if ( is_valid_ipv4( line ) )
		server.set_ip( line );
	else
		throw std::invalid_argument("Invalid IPv4 on address directive.");
}

void add_listen( std::string line, ConfigBase &item ) { 
	
	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Loop checking each port */
	std::istringstream stream( line );
	std::string portStr;

	while ( std::getline( stream, portStr, ' ' ) ) {

		if ( is_valid_port( portStr ) ) {

			/* Get integer from line */
			char *endptr;
			long port = strtol( portStr.c_str(), &endptr, 10 );

			/* Verify server has already that port */
			if ( server.has_port( static_cast<int>(port) ) ) continue;

			/* Add port to Server */
			server.add_port( static_cast<int>(port) );

		} else
			throw std::invalid_argument("Invalid port on listen directive.");
	}
	
}

void add_server_name( std::string line, ConfigBase &item ) { 
	/* Castear a server */
	(void)line;
	(void)item;
}

void add_root( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_client_max_body_size( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_error_page( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_index( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_autoindex( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_cgi_pass( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_return( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_methods( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;
}

void add_alias( std::string line, ConfigBase &item ) { 
	/* Castear a location */
	(void)line;
	(void)item;
}
