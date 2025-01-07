#include "../../include/methods.hpp"
#include "../../include/HTTPResponse.hpp"
#include <sys/stat.h>
#include <netinet/in.h>

/**
 * @brief Function to check the modes that an item/folder has
 * 
 * @param	path Path of the item
 * @param	data Struct where the item information is saved. If it
 * 					is null, no information is saved
 * @param	mode Mode to check on the item
 * 
 * @return true if the item has the mode; false otherwise. Also, if there is any problem
 * 			searching the item on the path, false is also returned
 */
static bool	check_mode( std::string path, struct stat *data, mode_t mode )
{
	struct stat aux;

	if (!data)
		data = &aux;

	/* Get the file info */
	if ( stat( path.c_str(), data ) == -1 )
		return false;
	
	/* Check the mode */
	return (data->st_mode & mode) != 0;
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

	std::cout << "Path recieved: " << path << std::endl;

	/* NOTE: 1. Check if the item has write mode; otherwise, return a 403 response */
	if (!check_mode( path, &file_data, S_IWUSR ))
	{
		/* Find the 403 page */
		std::string page = server.get_error_page( 403 );

		/* Send the data */
		delete_send( client, 403, page, request);
	}

	std::cout << "OKers" << std::endl;

	/* TODO: 2. Check what type of item is (file or folder) */

}
