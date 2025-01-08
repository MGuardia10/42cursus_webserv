#include "../../include/methods.hpp"
#include "../../include/HTTPResponse.hpp"
#include <sys/stat.h>
#include <netinet/in.h>
#include <dirent.h>
#include <unistd.h>

/**
 * @brief Function to check the modes that an item/folder has
 * 
 * @param	path Path of the item
 * @param	data Struct where the item information is saved. If it
 * 					is null, no information is saved
 * @param	mode Mode to check on the item
 * 
 * @return The function return a pair of bools: The first value indicates
 * 			if the file exists; if it does, the second value indicates
 *			if has the mode of the parameter
 */
static std::pair<bool, bool>	check_mode( std::string path, struct stat *data, mode_t mode )
{
	struct stat aux;

	if (!data)
		data = &aux;

	/* Get the file info */
	if ( stat( path.c_str(), data ) == -1 )
		return std::pair<bool, bool>(false, false);
	
	/* Check the mode */
	return std::pair<bool, bool>(true, (data->st_mode & mode) != 0);
}

static void	delete_send( Client& client, int code, std::string page_path, HTTPRequest *request )
{
	/* Remove the body file */
	request->remove_body_file();

	/* Read the page until the reads ends */
	std::pair<int, std::string> response;
	long long offset = 0;
	do
	{
		/* Get the page data */
		response = HTTPResponse::get_error_page_response( code, page_path, client.get_cookie(), offset );
		offset = response.first;

		/* Send the page fragment */
		send( client.get_fd(), response.second.c_str(), response.second.size(), 0);
	} while (offset != 0);
}

void	delete_method( std::string path, Client& client, HTTPRequest* request)
{
	struct stat file_data;
	const Server server = client.get_server();

	/* NOTE: 1. Check if the item has write mode; otherwise, return a 403 response */
	std::pair<bool, bool> file_status = check_mode( path, &file_data, S_IWUSR );
	if (!file_status.first)
	{
		delete_send( client, 404, server.get_error_page( 404 ), request );
		return ;
	}
	if (!file_status.second)
	{
		delete_send( client, 403, server.get_error_page( 403 ), request) ;
		return ;
	}

	/* NOTE: 2. Check what type of item is (file or folder) */
	if (!S_ISDIR(file_data.st_mode))
	{
		remove( path.c_str() );
		delete_send( client, 200, server.get_error_page( 200 ), request );
	}
	else
	{
		DIR* directory = opendir( path.c_str() );
		readdir(directory);
		readdir(directory);
		if (!readdir(directory))
		{
			rmdir( path.c_str() );
			delete_send( client, 200, server.get_error_page( 200 ), request );
		}
		else
			delete_send( client, 409, server.get_error_page( 409 ), request );

		closedir(directory);
	}

}
