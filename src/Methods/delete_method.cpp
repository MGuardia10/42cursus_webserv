#include "../../include/methods.hpp"
#include <dirent.h>
#include <unistd.h>

void	delete_method( std::string path, Client& client, HTTPRequest* request)
{
	struct stat file_data;
	std::pair<bool, const Location *> location = client.get_server().get_location( request->get_path() );

	/* NOTE: 1. Check if the item has write mode; otherwise, return a 403 response */
	std::pair<bool, bool> file_status = check_mode( path, &file_data, S_IWUSR );
	if (!file_status.first)
	{
		error_send( client, 404, location.second->get_error_page( 404 ), request );
		return ;
	}
	if (!file_status.second)
	{
		error_send( client, 403, location.second->get_error_page( 403 ), request) ;
		return ;
	}

	/* NOTE: 2. Check what type of item is (file or folder) */
	if (!S_ISDIR(file_data.st_mode))
	{
		remove( path.c_str() );
		error_send( client, 200, location.second->get_error_page( 200 ), request );
	}
	else
	{
		DIR* directory = opendir( path.c_str() );
		readdir(directory);
		readdir(directory);
		if (!readdir(directory))
		{
			rmdir( path.c_str() );
			error_send( client, 200, location.second->get_error_page( 200 ), request );
		}
		else
			error_send( client, 409, location.second->get_error_page( 409 ), request );

		closedir(directory);
	}

}
