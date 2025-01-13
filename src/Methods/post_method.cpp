#include "../../include/methods.hpp"

static std::string get_extension( std::string const &filename ) {

	// Find last occurrence of "."
    std::string::size_type pos = filename.rfind('.');
    
    // Check if "." exists and its not the first position
    if ( pos != std::string::npos && pos != 0 )
        return filename.substr(pos + 1);
    
    // If no valid extension found return empty string
    return "";

}

void	post_method( std::string path, Client& client, HTTPRequest* request)
{
	/* NOTE: 1. Get extension of file */
	std::string extension = get_extension( request->get_filename() );

	/* NOTE: 2. Get location from request and consult CGI's map */
	std::pair<bool, const Location *> location = client.get_server().get_location( request->get_path() );
	std::string cgi_found = location.second->get_cgi( extension );

	if ( cgi_found.empty() ) {

		/* NOTE: 2.1. Case is not CGI, so its a file */
		struct stat file_data;
		std::pair<bool, bool> file_status = check_mode( path, &file_data, S_IWUSR );

		/* Path does not exist */
		if ( !file_status.first )
			return error_send( client, 404, location.second->get_error_page( 404 ), request );

		/* Path does not have enough permissions (write and execute for upload files) */
		if ( !file_status.second )
			return error_send( client, 403, location.second->get_error_page( 403 ), request );

		/* If path end is a file get that filename, otherwise get name from get_filename function */
		std::string dest_filename;
		if ( !S_ISDIR(file_data.st_mode) )
			dest_filename = path;
		else {
			if ( !(file_data.st_mode & S_IXUSR) )
				return error_send( client, 403, location.second->get_error_page( 403 ), request );

			std::string new_path = ( path[ path.size() - 1 ] == '/' ) ? path : path + "/";

			if ( request->get_filename().find_last_of('/') == std::string::npos )
				dest_filename = new_path + request->get_filename();
			else
				dest_filename = new_path + request->get_filename().substr( request->get_filename().find_last_of('/') + 1 );

		}
			
		request->move_body_file( dest_filename );

	} else {

		/* NOTE: 2.2. Case is CGI */
		/* TODO */
	}


	std::cout << "path [" << path << "]" << '\n';

}
