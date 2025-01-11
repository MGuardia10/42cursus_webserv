#include "../../include/methods.hpp"

void	error_send( Client& client, int code, std::string page_path, HTTPRequest *request )
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