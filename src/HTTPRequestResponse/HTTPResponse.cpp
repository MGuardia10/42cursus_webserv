#include "../../include/HTTPResponse.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

std::map<int, std::string>	HTTPResponse::_codes;
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
/* SECTION:                           Loads                                   */
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

/** Function to load all the data necessary to prepare the responses */
bool	HTTPResponse::load_data( void )
{
	return load_codes() && load_extensions();
}

/** Function to load the errors types. It reads the info on the ERROR_PATH file. */
bool	HTTPResponse::load_codes( void )
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
	_codes.clear();
	for (std::map<std::string, std::string>::iterator it = res.second.begin(); it != res.second.end(); it++)
		_codes.insert(std::pair<int, std::string>(atoi(it->first.c_str()), it->second));

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

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                           Pages                                   */
/*============================================================================*/

/**
 * @brief Function to save the headers on a map as a string
 * 
 * @param	headers The headers on a map
 * 
 * @return The complete headers as string
 */
std::string	headers_map_to_string( std::map<std::string, std::string> headers )
{
	std::string header = "";

	std::map<std::string, std::string>::iterator it = headers.find("Status");
	if (it != headers.end())
		header += it->second + "\r\n";
	
	for (it = headers.begin(); it != headers.end(); it++)
		if (it->first != "Status")
			header += it->first + ": " + it->second + "\r\n";
	
	return header + "\r\n";
}

/** Function to get all the basic/general headers. */
std::map<std::string, std::string>	HTTPResponse::get_default_headers( int code, std::string cookie, bool connection_alive )
{
	std::map<std::string, std::string> headers;

	/* Status line */
	std::stringstream ss;
	std::map<int, std::string>::iterator code_search = _codes.find( code );

	ss << code;
	headers["Status"] = "HTTP/1.1 " + ss.str() + " " +
		(code_search != _codes.end() ? code_search->second : std::string("Undefined"));

	/* Server name */
	headers["Server"] = "Webserv/1.0";

	/* Date */
	time_t now_long = time(NULL);
	tm *now_struct = gmtime(&now_long);
	char now_string[80] = {0};
	strftime(now_string, 80, "%a, %d %b %Y %H:%M:%S GMT", now_struct);
	headers["Date"] = std::string(now_string);

	/* Connection */
	headers["Connection"] = (connection_alive ? std::string("keep-alive") : std::string("close"));

	/* Cookie */
	if (!cookie.empty())
		headers["Set-Cookie"] = cookie;

	/* Cache Control */
	headers["Cache-Control"] = "no-cache";

	return headers;
}

/** Function to generate a general response, with a specified code and msg */
std::string HTTPResponse::get_response_template( int code, std::string msg, std::string cookie )
{
	std::map<std::string, std::string> header;
	std::string	body;

	/* Get the default headers */
	header = get_default_headers( code, cookie, true );

	/* Set the body */
	body = "<html><head><title>Webserv</title></head><body><p>" + msg + "</p></body></html>";

	/* Add headers */
	std::stringstream ss;
	std::map<std::string, std::string>::iterator res = _extensions.find("html");
	ss << body.size();
	header["Content-Length"] = ss.str();
	header["Content-Type"] = (res == _extensions.end() ? "text/html" : res->second);

	/* Put everything together */
	return headers_map_to_string(header) + body;
}

/** Function that generates a response that indicates that the connection is closed */
std::string	HTTPResponse::get_close_connection_template( std::string cookie )
{
	std::map<std::string, std::string> header;

	/* General headers */
	header = get_default_headers( 200, cookie, false );

	return headers_map_to_string(header);
}

/** Function to return the data of a file */
std::pair<long long, std::string>	HTTPResponse::get_file_response( std::string path, std::string cookie, long long offset )
{
	std::map<std::string, std::string> header;
	std::string body, response;
	std::ifstream	file;

	/* Check if the file exists */
	file.open( path.c_str() );
	if (!file.is_open())
	{
		response = get_response_template( 404, "The resource has not been found", cookie);
		return std::pair<size_t, std::string>(-1, response);
	}

	/* If the offset is 0, generate the header */
	header.clear();
	if (offset == 0)
	{
		/* Default headers */
		header = get_default_headers( 200, cookie, true );

		/* File length */
		/* TODO: Check if it is a directory? */
		struct stat data;
		stat( path.c_str(), &data );

		std::stringstream	ss;
		ss << data.st_size;
		header["Content-Length"] = ss.str();

		/* File extension */
		std::map<std::string, std::string>::iterator it;
		size_t index = path.rfind(".");
		if (index != std::string::npos)
			 it = _extensions.find(path.substr(index + 1, path.size()));
		header["Content-Type"] = (index != std::string::npos && it != _extensions.end() ? it->second : "text/plain");
	}

	/* Read and write on the buffer */
	std::vector<char>	buffer(CHUNK_SIZE);
	file.seekg( offset );
	file.read(&buffer[0], CHUNK_SIZE);
	body = std::string(&buffer[0], buffer.size());

	size_t	new_offset = (file.gcount() == CHUNK_SIZE) ? offset + CHUNK_SIZE : 0;
	response = (header.empty()) ? body : headers_map_to_string( header ) + body;

	/* Return the response, plus headers if it is neccesary */
	return std::pair<size_t, std::string>( new_offset, response );
}

/** Function to generate the response of a return statement */
std::string	HTTPResponse::get_return_response( Server::ReturnData* data, std::string cookie )
{
	std::map<std::string, std::string> header;
	std::string response;

	if (!data)
		return "";

	/* Check if it is a redirection or a message */
	if (data->text.find("http://") != 0 && data->text.find("https://") != 0)
		response = get_response_template( data->code, data->text, cookie );
	else
	{
		/* Redirection */
		header = get_default_headers( data->code, cookie, false );
		header["Location"] = data->text;
		response = headers_map_to_string( header );
	}
	return response;
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
