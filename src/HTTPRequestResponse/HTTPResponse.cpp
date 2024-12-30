#include "../../include/HTTPResponse.hpp"
#include <sstream>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

HTTPResponse::HTTPResponse( void )
{}

HTTPResponse::~HTTPResponse( void )
{}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

/** Function to get all the basic/general headers. */
std::string	HTTPResponse::get_default_headers( int code, std::string cookie, bool end )
{
	std::string header;

	/* Status line */
	std::stringstream ss;
	ss << code;
	header = "HTTP/1.1 " + ss.str() + " OK\r\n";

	/* Server name */
	header += "Server: Webserv/1.0\r\n";

	/* Date */
	time_t now_long = time(NULL);
	tm *now_struct = gmtime(&now_long);
	char now_string[80] = {0};
	strftime(now_string, 80, "%a, %d %b %Y %H:%M:%S GMT", now_struct);
	header += "Date: " + std::string(now_string) + "\r\n";

	/* Connection */
	header += "Connection: keep-alive\r\n";

	/* Cookie */
	header += "Set-Cookie: " + cookie + "\r\n";

	/* Cache Control */
	header += "Cache-Control: no-cache\r\n";

	/* Check to add the header delimiter */
	if (end)
		header += "\r\n";

	return header;
}

std::string HTTPResponse::get_response_template( int code, std::string msg, std::string cookie )
{
	std::string header;
	std::string	body;

	/* Get the default headers */
	header = get_default_headers( code, cookie, false );

	/* Set the body */
	body = "<html><head><title>Webserv</title></head><body><p>" + msg + "</p></body></html>";

	/* Add headers */
	std::stringstream ss;
	ss << body.size();
	header += "Content-Length: " + ss.str() + "\r\n";
	header += "Content-Type: text/html\r\n"; /* FIXME: call a function to get this header */
	header += "\r\n";

	return header + body;
}

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
