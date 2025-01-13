#include "../../include/parse.hpp"
#include <set>
#include <climits>
#include <cerrno>

void add_address( std::string line, ConfigBase &item ) {
	
	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("address directive cannot be empty.");

	if ( line.compare("localhost") == 0 )
		server.set_ip( IP_DEFAULT );
	else if ( is_valid_ipv4( line ) )
		server.set_ip( line );
	else
		throw std::invalid_argument("Invalid address directive. Address must be a valid IPv4.");
}

void add_listen( std::string line, ConfigBase &item ) { 
	
	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

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
			throw std::invalid_argument("Invalid listen directive. Port must be a number between 0 to 65535.");
	}
	
}

void add_server_name( std::string line, ConfigBase &item ) {

	/* Cast to Server */
	Server &server = static_cast<Server &>( item );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("server_name directive cannot be empty.");

	/* Set new server name */
	server.set_server_name( line );

}

void add_root( std::string line, ConfigBase &item ) { 

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("root directive cannot be empty.");
	
	/* Check line is an absolute path */
	if ( !is_valid_absolute_path( line ) )
		throw std::invalid_argument("Invalid root directive. Root must be one absolute path.");

	// if ( line == "/" )
	// 	item.set_root( line );
	// else
	line.at( line.size() - 1 ) != '/' ? item.set_root( line ) : item.set_root( line.substr( 0, line.size() - 1 ) );

}

void add_client_max_body_size( std::string line, ConfigBase &item ) {

	/* Check line is empty or line starts with "-" */
	if ( line.empty() || line.at(0) == '-' )
		throw std::invalid_argument("client_max_body_size directive cannot be empty or a negative number.");

	/* Get bytes from line and endptr */
	char* endptr;
    size_t bytes = strtoul( line.c_str(), &endptr, 10 );

	/* Check if the value is too large for unsigned long */
	if (bytes == ULONG_MAX && errno == ERANGE)
		throw std::invalid_argument("client_max_body_size value is too large.");

	/* Convert endptr to std::string */
	std::string suffix( endptr );

	/* If no suffix, add bytes and return */
	if ( suffix.size() == 0 ) {

		/* Check limit for 50 MB */
		if ( bytes > MAX_BODY_SIZE_BYTES )
			throw std::invalid_argument("client_max_body_size exceeds the maximum limit of 50 MB.");
		
		item.set_client_max_size( bytes );
		return ;
	}

	/* Case invalid suffix */
	if ( suffix.size() != 1 )
		throw std::invalid_argument("Invalid client_max_body_size directive. Accepted suffix are [ k, K, m, M, g, G ].");

	/* Multiply bytes based on suffix */
	size_t multiplier = 0;
	switch ( std::tolower( suffix[0] ) ) {
	
	case 'k':
		/* Case Kilobytes */
		multiplier = 1024;
		break;

	case 'm':
		/* Case Megabytes */
		multiplier = 1024 * 1024;
		break;

	case 'g':
		/* Case Gigabytes */
		multiplier = 1024 * 1024 * 1024;
		break;
	
	default:
		throw std::invalid_argument("Invalid client_max_body_size directive. Accepted suffix are [ k, K, m, M, g, G ].");
	}

	/* Protect overflow cases */
    if ( bytes > ULONG_MAX / multiplier ) {
        throw std::invalid_argument("client_max_body_size value causes overflow.");
    }

	size_t total = bytes * multiplier;

	/* Check limit for 50 MB for total */
	if ( total > MAX_BODY_SIZE_BYTES )
		throw std::invalid_argument("client_max_body_size exceeds the maximum limit of 50 MB.");

	/* Set client_max_body_size */
	item.set_client_max_size( total );

}

void add_error_page( std::string line, ConfigBase &item ) { 

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("error_page directive cannot be empty.");

	/* Check line has valid code and file */
	std::istringstream stream( line );
	std::string token;

	std::vector<int> codes;
	std::string redirect;

	while ( stream >> token ) {

		/* Check is valid code */
		int code;
		if ( ( code = http_code( token ) ) != -1 && code >= 300 && code <= 599 ) {
			/* valid code found */
			codes.push_back( code );

		} else if ( is_valid_url_or_path( token ) ) {
			/* Keep redirect string */
			redirect = token;
			
			/* Exit loop */
			break;

		} else
			throw std::invalid_argument("Invalid error_page directive. Format must follow: error_page [error_codes (300 - 599)] [path | URL];");
	}

	/* Check codes, redirect and nothing to read behind */
	if ( codes.empty() || redirect.empty() || stream >> token )
		throw std::invalid_argument("Invalid error_page directive. Format must follow: error_page [error_codes (300 - 599)] [path | URL];");

	/* Push codes and redirect to item */
	for ( std::vector<int>::iterator it = codes.begin(); it != codes.end(); ++it ) {
		item.add_error_page( *it , redirect );
	}
}

void add_index( std::string line, ConfigBase &item ) { 

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("index directive cannot be empty.");

	/* Loop index by index */
	std::istringstream stream( line );
	std::string indexStr;

	while ( std::getline( stream, indexStr, ' ' ) ) {
		
		/* Discard absolute paths and directories */
		if ( indexStr.at( 0 ) == '/' )
			throw std::invalid_argument("Invalid index directive. Index cannot be an absolute path.");
		if ( indexStr.at( indexStr.size() - 1 ) == '/' )
			throw std::invalid_argument("Invalid index directive. Index cannot be a directory.");
		
		/* Add index to item */
		item.add_index( indexStr );
	}
}

void add_autoindex( std::string line, ConfigBase &item ) { 

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("autoindex directive cannot be empty.");

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
		throw std::invalid_argument("Invalid autoindex directive. Accepted values are [ true, false ].");
}

void add_cgi_pass( std::string line, ConfigBase &item ) { 

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("cgi_pass directive cannot be empty.");

	/* Check line has valid extension and path */
    std::istringstream stream( line );
	std::string extension, path;

	if ( std::getline( stream, extension, ' ' ) && std::getline( stream, path, ' ' ) ) {

		/* Check valid file extension */
		if ( extension.find_first_of( "./" ) != std::string::npos )
			throw std::invalid_argument("Invalid cgi_pass directive. Invalid file extension.");

		/* Check valid path to file */
		if ( path.at(0) != '/' || path.at( path.size() - 1 ) == '/' )
			throw std::invalid_argument("Invalid cgi_pass directive. Invalid path to binary.");

		/* Add new CGI to item */
		item.add_cgi( extension, path );

		/* Check end of string */
		if ( std::getline( stream, extension, ' ' ) )
			throw std::invalid_argument("Invalid cgi_pass directive. CGI must contain exactly extension file and path to binary.");

	}
	else
		throw std::invalid_argument("Invalid cgi_pass directive. CGI must contain exactly extension file and path to binary.");

}

void add_return( std::string line, ConfigBase &item ) { 

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("return directive cannot be empty.");

	/* Check is valid line */
	std::istringstream stream( line );
	std::string code, text;

	if ( std::getline( stream, code, ' ' ) && std::getline( stream, text ) ) {

		/* ReturnData type variable */
		ConfigBase::ReturnData data;

		/* Check valid http code */
		if ( ( data.code = http_code( code ) ) == -1 )
			throw std::invalid_argument("Invalid return directive. Return code must be a valid HTTP code (between 100 and 599).");

		/* Check valid URL/text */
		if ( text.at(0) != '"' || text.at( text.size() - 1 ) != '"' )
			throw std::invalid_argument("Invalid return directive. Return text must be all between quotes.");

		/* Remove quotes from text to insert on data */
		data.text = text.substr( 1, text.size() - 2 );

		/* Set return data on item */
		item.set_return( data );
	}
	else
		throw std::invalid_argument("Invalid return directive. Return directive contains exactly one status code and one URL/text.");
	
}

void add_methods( std::string line, ConfigBase &item ) {

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("methods directive cannot be empty.");

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
			throw std::invalid_argument("Invalid methods directive. Accepted methods are [ GET, POST, DELETE ].");

		/* Case valid method is found, push to server */
		item.add_method( methodStr );
	}
}

void add_alias( std::string line, ConfigBase &item ) { 
	
	/* Cast to location */
	Location &location = static_cast<Location &>( item );

	/* Check line is empty */
	if ( line.empty() )
		throw std::invalid_argument("alias directive cannot be empty.");

	/* Check line is an absolute path */
	if ( !is_valid_absolute_path( line ) )
		throw std::invalid_argument("Invalid alias directive. Alias must be one absolute path.");

	/* Push alias to location */
	if ( line == "/" )
		location.set_alias( line );
	else
		line.at( line.size() - 1 ) != '/' ? location.set_alias( line ) : location.set_alias( line.substr( 0, line.size() - 1 ) );

}
