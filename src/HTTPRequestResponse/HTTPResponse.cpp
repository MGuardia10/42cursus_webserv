#include "../../include/HTTPResponse.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

std::map<int, std::string>	HTTPResponse::_errors;
std::map<std::string, std::string>	HTTPResponse::_extensions;

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

/* NOTE: Loads */

/**
 * @brief Generic function to read de csv config files
 * 
 * @param	file Reference to the config file
 * @param	delimiter	Delimited of the rows
 * 
 * @return A pair with false and an empty map if there is any problem during the reading;
 * 			Otherwise, true and the map with the data
 */
static std::pair< bool, std::map<std::string, std::string> > read_csv_files( std::ifstream& file, std::string delimiter )
{
	std::map<std::string, std::string> data;
	std::string	buffer;
	std::string first, second;
	size_t	index;

	getline(file, buffer);
	while (getline(file, buffer))
	{
		if (buffer.empty() || buffer[0] == '#')
			continue ;
		
		/* Search the delimiter */
		index = buffer.find(delimiter);
		if (index == std::string::npos)
			return std::pair< bool, std::map<std::string, std::string> >(false, std::map<std::string, std::string>());
		
		/* Get the two parts */
		first = buffer.substr(0, index);
		second = buffer.substr(index + 1, buffer.size());
		if (first.empty() || second.empty())
			return std::pair< bool, std::map<std::string, std::string> >(false, std::map<std::string, std::string>());

		/* Insert the parts */
		data.insert(std::pair<std::string, std::string>(first, second));
	}
	return std::pair< bool, std::map<std::string, std::string> >(true, data);
}

/** Function to load the errors types. It reads the info on the ERROR_PATH file. */
bool	HTTPResponse::load_errors( void )
{
	std::ifstream	file;

	file.open( ERRORS_PATH );
	if (!file.is_open())
		return false;
	
	/* Get the data */
	std::pair< bool, std::map<std::string, std::string> > res = read_csv_files(file, ",");
	file.close();
	if (!res.first)
		return false;
	
	/* Save the data */
	_errors.clear();
	for (std::map<std::string, std::string>::iterator it = res.second.begin(); it != res.second.end(); it++)
		_errors.insert(std::pair<int, std::string>(atoi(it->first.c_str()), it->second));

	return true;
}

/** Function to load the errors types. It reads the info on the EXTENSIONS_PATH file. */
bool	HTTPResponse::load_extensions( void )
{
	std::ifstream	file;

	file.open( EXTENSIONS_PATH );
	if (!file.is_open())
		return false;
	
	/* Get the data */
	std::pair< bool, std::map<std::string, std::string> > res = read_csv_files(file, ",");
	file.close();
	if (!res.first)
		return false;
	
	/* Save the data */
	_extensions = res.second;

	return true;
}

/* NOTE: pages */

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
	std::map<std::string, std::string>::iterator res = _extensions.find("html");
	ss << body.size();
	header += "Content-Length: " + ss.str() + "\r\n";
	header += "Content-Type: " + (res == _extensions.end() ? "text/html" : res->second) + "\r\n";

	/* Put everything together */
	return header + "\r\n" + body;
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
