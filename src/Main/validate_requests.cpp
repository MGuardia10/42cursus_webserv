#include "../../include/validate_requests.hpp"

bool	is_supported_HTTP_method( std::string method ) {
	return method.compare( "GET" ) == 0 || method.compare( "POST" ) == 0 || method.compare( "DELETE" ) == 0;
}

void	validate_request( Client client, HTTPRequest request ) {
	
	/* 1. Create HTTPResponse object */
	// HTTPResponse response;

	/* 2. Check protocol is HTTP/1.1 */
	if ( request.get_method().compare( "HTTP/1.1" ) != 0 ) {
		// HTTPResponse = 505
		// return response;
		return ;
	}

	/* 3. Check route */
	Server clientServer = client.get_server();
	std::pair<bool, const Location *> location = clientServer.get_location( request.get_path() );
	if ( !location.first ) {
		// HTTPResponse = 400 -> BUscar index.html o en su defecto hardcoded error
		// return response;
		return ;
	}

	/* 3.5 Verificar tamaño body */


	/* 4. Check there is return */
	ConfigBase::ReturnData returnData = location.second->get_return();
	if ( returnData.code != -1 ) {
		/* Si hay return, comprobar max_body_size */
		// if ( returnData.text.size() >  )
		
	}

	/* 5. Check request method */
	if ( !is_supported_HTTP_method( request.get_method() ) ) {
		// HTTPresponse = 405 -> Buscar index.html para error o hardcoded error
		// HTTP/1.1 incluye header "allow" y lista de metodos validos {GET,POST,DELETE}
		// return response;
		return ;
	}

	/* 6. Build path to resource */

}