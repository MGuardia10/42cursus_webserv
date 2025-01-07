#include "../../include/methods.hpp"
#include "../../include/HTTPResponse.hpp"
#include <sys/stat.h>

/**
 * @brief Function to check the modes that an item/folder has
 * 
 * @param	path Path of the item
 * @param	data Struct where the item information is saved. If it
 * 					is null, no information is saved
 * @param	mode Mode to check on the item
 * 
 * @return true if the item has the mode; false otherwise
 */
static bool	check_mode( std::string path, struct stat *data, mode_t mode )
{
	struct stat aux;

	if (!data)
		data = &aux;

	/* Get the file info */
	stat( path.c_str(), data );
	
	/* Check the mode */
	return (data->st_mode & mode) != 0;
}

static void	delete_send( int fd, std::string response )
{

}

void	delete_method( std::string path, Client& client, HTTPRequest* request)
{
	(void) path, (void) client, (void) request;
	struct stat file_data;

	const Server server = client.get_server();
	

	/* NOTE: 1. Check if the item has write mode; otherwise, return a 403 response */
	if (check_mode( path, &file_data, S_IWUSR ) == false)
	{
		/* Find the 403 page */
		std::string page = server.get_error_page( 403 );
		/* FIXME: the page should be find for path minus the request path or something like that */
	}


	/* TODO: 2. Check what type of item is (file or folder) */

}
