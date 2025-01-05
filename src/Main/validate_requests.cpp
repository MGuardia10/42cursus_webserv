#include "../../include/validate_requests.hpp"
#include <cstdlib>

bool	is_supported_HTTP_method( std::string method ) {
	return method.compare( "GET" ) == 0 || method.compare( "POST" ) == 0 || method.compare( "DELETE" ) == 0;
}

std::string build_error_page_path( const ConfigBase& item, int code ) {
	
	std::string path = "";
	std::string error_file = item.get_error_page( code );

	/* Return empty path if there is no error page */
	if ( error_file.empty() ) return path;

	/* Otherwise, build path */
	if ( Server const* server = dynamic_cast<const Server*>( &item ) ) {
        /* Case item is a server, no ALIAS */
		path = server->get_root() + error_file;

    } else if ( Location const* location = dynamic_cast<const Location*>( &item ) ) {
        /* Case item is a location, check ALIAS */
		if ( location->get_alias().empty() ) {
			std::string basePath = location->get_root() + location->get_route();
			path = basePath + error_file;
		}
		else {
			std::string basePath = location->get_root() + location->get_alias();
			path = basePath + error_file;
		}
    }

	/* DEBUG: path to error_page */
	std::cout << "PATH ERROR: {" << path << "}\n";

	/* Return complete path to error file */
	return path;
}

RequestData	validate_request( Client client, HTTPRequest* request ) {
	
	/* 1. Get Client's server, and initialize RequestData values */
	Server const clientServer = client.get_server();
	RequestData request_data;
	request_data.errorData.code = -1;
	request_data.errorData.path = "";
	request_data.returnData.code = -1;
	request_data.returnData.text = "";

	/* 2. Check protocol is HTTP/1.1 */
	if ( request->get_protocol().compare( "HTTP/1.1" ) != 0 ) {
		
		/* Set errorData code to 505 */
		request_data.errorData.code = 505;

		/* Buscar si existe ruta para ese error */
		request_data.errorData.path = build_error_page_path( clientServer, request_data.errorData.code );

		/* return resquestData struct */
		return request_data;
	}

	/* 3. Check request path to a Location */
	std::pair<bool, Location const*> location = clientServer.get_location( request->get_path() );
	if ( !location.first ) {

		/* Set errorData code to 400 */
		request_data.errorData.code = 400;

		/* Build error page path if found */
		request_data.errorData.path = build_error_page_path( clientServer, request_data.errorData.code );

		/* return resquestData struct */
		return request_data;
	}

	/* 4 Verificar tamaño body */
	std::pair<bool, std::string> content_length_header = request->find_header( "Content-Length" );
	if ( content_length_header.first ) {
		
		/* If header exists, check length < client_max_boxy_size */
		long long max_size = location.second->get_client_max_size();
		long long current_size = atoll( content_length_header.second.c_str() );

		if ( current_size > max_size ) {

			/* Set errorData code to 412 */
			request_data.errorData.code = 412;

			/* Buscar si existe ruta para ese error */
			request_data.errorData.path = build_error_page_path( *location.second, request_data.errorData.code );

			/* return resquestData struct */
			return request_data;
		}
	}

	/* 5. Check there is return */
	request_data.returnData = location.second->get_return();
	if ( request_data.returnData.code != -1 ) {
		/* Return found on location, returning request_data */
		return request_data;
	}

	/* 6. Check request method */
	if ( !is_supported_HTTP_method( request->get_method() ) ) {
		// HTTP/1.1 incluye header "allow" y lista de metodos validos {GET,POST,DELETE} ???????????
		/* Set errorData code to 405 */
		request_data.errorData.code = 405;

		/* Buscar si existe ruta para ese error */
		request_data.errorData.path = build_error_page_path( *location.second, request_data.errorData.code );

		/* return resquestData struct */
		return request_data;
	}

	return request_data;

}