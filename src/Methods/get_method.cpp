#include "../../include/methods.hpp"

void	get_send( int code, std::string& path, Client& client,  HTTPRequest* request ) {

	/* Remove the body file */
	request->remove_body_file();

	long long offset = 0;
	std::string response;

	/* Loop until offset 0 again */
	do
	{
		/* Get reponse */
		std::pair<long long, std::string> data = HTTPResponse::get_file_response( code, path, client.get_cookie(), offset );
		
		/* Update values */
		offset = data.first;
		response = data.second;

		// std::cout << "response: $$$$$$$$$$$$$$$$$$\n";
		// std::cout << response <<"\n";
		// std::cout << "$$$$$$$$$$$$$$$$$$\n\n";

		/* Send response */
		if (send(client.get_fd(), response.c_str(), response.size(), MSG_NOSIGNAL) == -1)
			break ;

	} while (offset != 0);

}

void	get_method( std::string path, Client& client, HTTPRequest* request)
{
	struct stat file_data;
	std::pair<bool, const Location *> location = client.get_server().get_location( request->get_path() );

	std::cout << "path [" << path << "]" << '\n';

	/* NOTE: 1. Check if the item exists and has lecture mode; otherwise, return a 404 (Not Found) or 403 (forbidden) response */
	std::pair<bool, bool> file_status = check_mode( path, &file_data, S_IRUSR );
	if (!file_status.first)
		return error_send( client, 404, location.second->get_error_page( 404 ), request );
	if (!file_status.second)
		return error_send( client, 403, location.second->get_error_page( 403 ), request );

	/* NOTE: 2. Check what type of item is (file or folder) */
	if ( !S_ISDIR(file_data.st_mode) ) {

		/* NOTE: 2.1. Case is a file, send get response */
		get_send( 200, path, client, request );
		
	}
	else {
		/* NOTE: 2.2. Case is a folder: Find index first, autoindex second */

		/* NOTE: 2.3. Find index file to folder path */
		std::vector<std::string> indexes = location.second->get_indexes();

		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			
			struct stat file_data;
			
			std::pair<bool, bool> index_status = check_mode( *it, &file_data, S_IRUSR );
			if ( index_status.first && index_status.second ) {

				/* If index found and permissions OK, get response to that index path */
				return get_send( 200, *it, client, request );

			}

   		}

		/* NOTE: 2.4. Check autoindex */
		if ( location.second->get_autoindex() ) {

			std::string autoindex_response = HTTPResponse::get_autoindex_response( path, client.get_cookie() );

			/* Send autoindex resonse */
			send( client.get_fd(), autoindex_response.c_str(), autoindex_response.size(), MSG_NOSIGNAL );
			return ;
		}

		/* NOTE: 2.5. Case Bad request to a folder path */
		error_send( client, 403, location.second->get_error_page( 403 ), request );
		
	}
}
