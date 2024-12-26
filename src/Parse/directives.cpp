#include "../../include/parse.hpp"
#include <set>

void add_address( std::string line, ConfigBase &item ) {
	
	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("address directive cannot be empty.");

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

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("listen directive cannot be empty.");

	/* Loop checking each port */
	std::istringstream stream( line );
	std::string portStr;

	while ( std::getline( stream, portStr, ' ' ) ) {

		if ( is_valid_port( portStr ) ) {

			/* Get integer from line */
			char *endptr;
			long port = strtol( portStr.c_str(), &endptr, 10 );

			/* Add port to Server */
			server.add_port( static_cast<int>(port) );

		} else
			throw std::invalid_argument("Invalid port on listen directive.");
	}
	
}

void add_server_name( std::string line, ConfigBase &item ) {

	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("server_name directive cannot be empty.");

	/* Set new server name */
	server.set_server_name( line );

}

void add_root( std::string line, ConfigBase &item ) { 
	
	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("root directive cannot be empty.");
	
	/* Check line is an absolute path */
	if ( !is_valid_absolute_path( line ) )
		throw std::invalid_argument("Invalid root directive. Root must be one absolute path.");

	line.at( line.size() - 1 ) != '/' ? item.set_root( line ) : item.set_root( line.substr( 0, line.size() - 1 ) );

}

void add_client_max_body_size( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("client_max_body_size directive cannot be empty.");
}

void add_error_page( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("error_page directive cannot be empty.");
}

void add_index( std::string line, ConfigBase &item ) { 
	
	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("index directive cannot be empty.");

	/* Loop index by index */
	std::istringstream stream( line );
	std::string indexStr;

	while ( std::getline( stream, indexStr, ' ' ) ) {
		
		/* Discard absolute paths and directories */
		if ( indexStr.at( 0 ) == '/' )
			throw std::invalid_argument("Invalid file on index directive. Index cannot be an absolute path.");
		if ( indexStr.at( indexStr.size() - 1 ) == '/' )
			throw std::invalid_argument("Invalid file on index directive. Index cannot be a directory.");
		
		/* Add index to item */
		item.add_index( indexStr );
	}
}

void add_autoindex( std::string line, ConfigBase &item ) { 
	
	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("Invalid value on autoindex directive. Accepted values are [ true, false ].");

	/* Convert all chars to lowercase */
    std::string lowerCaseLine;
    for ( std::string::const_iterator it = line.begin(); it != line.end(); ++it ) {
        lowerCaseLine += std::tolower( *it );
	}

	/* Set autoindex */
	if ( lowerCaseLine.compare( "true" ) == 0 )
		item.set_autoindex( true );
	else if ( lowerCaseLine.compare( "false" ) == 0 )
		item.set_autoindex( false );
	else
		throw std::invalid_argument("Invalid value on autoindex directive. Accepted values are [ true, false ].");
}

void add_cgi_pass( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("cgi_pass directive cannot be empty.");
}

void add_return( std::string line, ConfigBase &item ) { 
	(void)line;
	(void)item;

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("return directive cannot be empty.");
}

void add_methods( std::string line, ConfigBase &item ) { 
	
	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("No method found on methods directive.");

	/* Valid methods for our Server */
	std::set<std::string> validMethods;
	validMethods.insert( "GET" );
    validMethods.insert( "POST" );
    validMethods.insert( "DELETE" );

	/* Convert all chars to uppercase */
    std::string upperCaseLine;
    for ( std::string::const_iterator it = line.begin(); it != line.end(); ++it ) {
        upperCaseLine += std::toupper( *it );
	}

	/* Loop through all items on line separated by a space */
	std::istringstream stream( upperCaseLine );
	std::string methodStr;
	while ( std::getline( stream, methodStr, ' ' ) ) {

		if ( validMethods.find( methodStr ) == validMethods.end() )
			throw std::invalid_argument("Invalid method on methods directive. Accepted methods are [ GET, POST, DELETE ].");

		/* Case valid method is found, push to server */
		item.add_method( methodStr );
	}
}

void add_alias( std::string line, ConfigBase &item ) { 
	
	/* Cast to location */
	Location &location = static_cast<Location &>( item );

	/* normalize line without directive key and semicolon */
	normalize_string( line );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("alias directive cannot be empty.");

	/* Check line is an absolute path */
	if ( !is_valid_absolute_path( line ) )
		throw std::invalid_argument("Invalid alias directive. Alias must be one absolute path.");

	/* Push alias to location */
	line.at( line.size() - 1 ) != '/' ? location.set_alias( line ) : location.set_alias( line.substr( 0, line.size() - 1 ) );

}
