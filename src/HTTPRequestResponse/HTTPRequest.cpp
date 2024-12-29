#include "../../include/HTTPRequest.hpp"
#include "../../include/signals.hpp"
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

# define HTTP_DELIMITIER "\r\n"
# define BUFFER_SIZE 1025
// # define FILENAME_TAG	"name"
# define FILENAME_TAG	"filename"


/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

size_t HTTPRequest::REQUESTS_COUNT = 0;

HTTPRequest::HTTPRequest( void ) :
	_request(""),
	_method(""),
	_path(""),
	_protocol(""),
	_is_closed(false),
	_is_finished(false),
	_boundary(""),
	_request_filename(""),

	_content_length(-1),
	_body_started(false),
	_body_headers_cleaned(false)
{
	_headers.clear();
	_body.clear();

	/* File config */
	std::stringstream ss;
	ss << HTTPRequest::REQUESTS_COUNT++;
	_body_filename = "/tmp/outngnx_request_" + ss.str();
	_file.open(_body_filename.c_str(), std::ios::binary);
}

HTTPRequest::~HTTPRequest( void )
{}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/

std::string	HTTPRequest::print( void ) const
{
	std::string buffer = "";

	/* First line info */
	buffer += "[ REQUEST ]\n";
	buffer += "\t- Method: \"" + _method + "\"\n";
	buffer += "\t- Path: \"" + _path + "\"\n";
	buffer += "\t- Protocol: \"" + _protocol + "\"\n";

	/* Headers */
	buffer += "\t- Headers: ";
	if (_headers.size() == 0)
		buffer += "None";
	buffer += "\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		buffer += "\t\t+ " + it->first + "=" + it->second + "\n";
	
	/* Information about the body file */
	buffer += "\t- Body file path: \"" + _body_filename + "\"\n";

	/* Information about the request status */
	buffer += "\t- Request finished: " + (_is_finished ? std::string("Yes\n") : std::string("No\n"));
	buffer += "\t- Connection closed: " + (_is_closed ? std::string("Yes\n") : std::string("No\n"));

	return buffer;
}

std::ostream&	operator<<( std::ostream& os, HTTPRequest const& printObject )
{
	os << printObject.print();
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

std::string	HTTPRequest::get_method( void ) const { return _method; }
std::string	HTTPRequest::get_path( void ) const { return _path; }
std::string	HTTPRequest::get_protocol( void ) const { return _protocol; }
std::map<std::string, std::string>	HTTPRequest::get_headers( void ) const { return _headers; }
bool		HTTPRequest::check_closed( void ) const { return _is_closed; }
bool		HTTPRequest::check_finished( void ) const { return _is_finished; }
std::string	HTTPRequest::get_filename( void ) const { return _body_filename; }

std::pair<bool, std::string>	HTTPRequest::find_header( std::string header ) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find( header );
	return it == _headers.end()
					? std::pair<bool, std::string>(false, "")
					: std::pair<bool, std::string>(true, it->second);
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

/**
 * @brief	Function to split and save all the headers provided. The information
 * 			splitted is:
 * 				· The first line (method, path, protocol)
 * 				· Headers, divided by ": ", so the information is saved as
 * 					key-value on a map
 * 
 * @param[in]	headers String with all the headers together
 * 
 * @return void
 */
void	HTTPRequest::parse_headers(std::string headers)
{
	size_t	index;
	std::string current_line;

	while (!headers.empty())
	{
		index = headers.find(HTTP_DELIMITIER);
		current_line = headers.substr(0, index);
		headers = headers.substr(index + 2, headers.size());

		if (_method == "" || _path == "" || _protocol == "")
		{
			index = current_line.find(" ");
			if (index == std::string::npos)
				throw HTTPRequestException("Malformed first line of the header");
			_method = current_line.substr(0, index);
			current_line = current_line.substr(index + 1, current_line.size());

			index = current_line.find(" ");
			if (index == std::string::npos)
				throw HTTPRequestException("Malformed first line of the header");
			_path = current_line.substr(0, index);
			_protocol = current_line.substr(index + 1, current_line.size());
		}
		else
		{
			index = current_line.find(": ");
			if (index == std::string::npos)
				throw HTTPRequestException("Malformed header");
			_headers.insert(std::pair<std::string, std::string>(
				current_line.substr(0, index),
				current_line.substr(index + 2, current_line.size())
			));
		}
	}
}

/**
 * @brief Function to get the body length
 * 
 * @param	headers Map with all the headers
 * 
 * @return The body size to take into account
 */
static size_t	get_content_length( std::map<std::string, std::string>& headers)
{
	std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
	return it == headers.end() ? 0 : atoll(it->second.c_str());
}

/**
 * @brief Function to extract the boundary, the body delimiter.
 * 
 * @param	headers Map ith all the headers
 * 
 * @return The delimiter to take into account
 */
static std::string	get_boundary( std::map<std::string, std::string>& headers )
{
	std::map<std::string, std::string>::iterator it = headers.find("Content-Type");
	if (it == headers.end())
		return "";

	/* Check if the boundary is on the header */
	std::string header = it->second;
	size_t	index = header.find("boundary=");
	if (index == std::string::npos)
		return "";
	
	/* The boundary is on the header: extract it */
	return header.substr(index + 9, header.size());
}

/** Function to get and process a HTML request */
void	HTTPRequest::process_request( int fd )
{
	/* Buffers */
	char		buffer[BUFFER_SIZE];	/* Lecture buffer */
	int			bytes_received;			/* Length of the read fragment */

	size_t		headers_size;	/* Index to check the end of the headers part */
	std::string body_string;	/* Aux variable to have the body as string */

	while (_content_length != 0)
	{
		/*
			Received the request and save it correctly.
			Also check if the request is fragment or if the connection is closed
		*/
		bytes_received = recv(fd, buffer, BUFFER_SIZE - 1, 0);
		if (bytes_received < 0)
			break ;
		if (bytes_received == 0)
		{
			_is_closed = true;
			_is_finished = true;
			_file.close();
			remove(_body_filename.c_str());
			return ;
		}
		buffer[bytes_received] = '\0';

		/* Check what has to be saved, the headers or the request body */
		if (!_body_started)
		{
			_request.append(buffer, bytes_received);
			headers_size = _request.find("\r\n\r\n");
			if (headers_size == std::string::npos)
				continue ;
			_body_started = true;

			/* Save the first line and headers */
			parse_headers(_request.substr(0, headers_size + 2));

			/* Search the body */
			_boundary = get_boundary( _headers );
			_content_length = get_content_length( _headers );
			if (!_content_length)
			{
				_file.close();
				remove(_body_filename.c_str());
				_body_filename = "";
			}

			/* Save the rest of the request on the body vector */
			if (_content_length && _request.size() > headers_size + 4)
				_body.insert(_body.end(), _request.begin() + headers_size + 4, _request.end());
		}
		else
			_body.insert(_body.end(), buffer, buffer + bytes_received);

		/* If there is no body, or the body hasn't started, go next iteration */
		if (_content_length == 0 || (_boundary != "" && _content_length - _boundary.size() - 6 == 0))
			continue ;

		/* If there is boundary, the parse is different */
		if (_boundary == "")
		{
			_file.write(&_body[0], _body.size());
			_content_length -= _body.size();
			_body.clear();
		}
		else
		{
			if (!_body_headers_cleaned)
			{
				/* Search of the boundaries */
				body_string = std::string(&_body[0], _body.size());
				size_t boundary_index = body_string.find(_boundary + "\r\n");
				size_t	next_boundary = 0;
				while (boundary_index != std::string::npos && next_boundary != std::string::npos)
				{
					/* Clean the precious data */
					_content_length -= boundary_index;
					_body.erase(_body.begin(), _body.begin() + boundary_index);
					body_string = std::string(&_body[0], _body.size());

					next_boundary = body_string.find(_boundary + "\r\n", 1);
					if (next_boundary != std::string::npos)
						boundary_index = next_boundary;
				}

				/* The string should now start with the last boundary and, therefore, the content-disposition with the name to put */
				/* Search of FILENAME_TAG, get the value, and erase all until \r\n */
				if (_request_filename == "")
				{
					size_t tag_index = body_string.find(FILENAME_TAG);
					if (tag_index == std::string::npos)
						continue ;

					/* Get the value */
					size_t end_tag_index = body_string.find("\"", tag_index + std::string(FILENAME_TAG).size() + 2);
					if (end_tag_index == std::string::npos)
						continue ;
					
					size_t start_tag = tag_index + std::string(FILENAME_TAG).size() + 2;
					_request_filename = body_string.substr(start_tag, end_tag_index - start_tag);
					size_t	end_body_headers = body_string.find("\r\n\r\n", end_tag_index);
					_content_length -= (end_body_headers + 4);
					_body.erase(_body.begin(), _body.begin() + end_body_headers + 4);
				}
				_body_headers_cleaned = true;
			}

			/* Save only the file, not the last header */
			size_t body_size_without_end = std::min(_content_length - _boundary.size() - 6, _body.size());
			_file.write(&_body[0], body_size_without_end);
			_content_length -= body_size_without_end;
			_body.clear();
		}
		
	}

	_is_finished = (_content_length == 0 || (_boundary != "" && _content_length - _boundary.size() - 6 == 0));

	/* Last process details */
	if (_is_finished)
	{
		/* Close the body file */
		if (_file.is_open())
			_file.close();
		
		/* Change names */
		if (_request_filename != "")
			move_body_file("/tmp/" + _request_filename);

		/* Check if the connection is closed by the header */
		std::map<std::string, std::string>::iterator map_it = _headers.find("Connection");
		if (map_it != _headers.end() && map_it->second == "close")
			_is_closed = true;
	}
}

/** Function to move the body file to a specific path */
void	HTTPRequest::move_body_file(std::string const& dest_path)
{
	/* Check if there is any file */
	if (_body_filename != "")
	{	
		/* Remove the dest file */
		remove(dest_path.c_str());

		/* Change the name of out current file */
		std::rename(_body_filename.c_str(), dest_path.c_str());

		/* Save the new name */
		_body_filename = dest_path;
	}
}

/** Function to remove the body file */
void	HTTPRequest::remove_body_file( void )
{
	if (_body_filename != "")
		remove(_body_filename.c_str());
}

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

HTTPRequest::HTTPRequestException::HTTPRequestException( std::string msg ) throw() :
	_msg(msg)
{}

HTTPRequest::HTTPRequestException::~HTTPRequestException( void ) throw()
{}

const char* HTTPRequest::HTTPRequestException::what( void ) const throw()
{
	return _msg.c_str();
}

/*==========*/
/* !SECTION */
/*==========*/
