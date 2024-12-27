#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>


/*
	POST / HTTP/1.1
	Host: localhost:8080
	User-Agent: curl/7.68.0
	Accept: * / *
	Content-type: application/json
	Content-Length: 16\r\n
	\r\n
	{"name": "Test"}
*/

class HTTPRequest
{
	private:
		static size_t	REQUESTS_COUNT;

		std::string	_request;

		/* First line */
		std::string	_method;	/* POST */
		std::string	_path;		/* / */
		std::string	_protocol;	/* HTTP/1.1 */

		/* Headers */
		std::map<std::string, std::string>	_headers;	/* Host: ..., Content-type: ...*/

		/* Connection dat */
		bool	_is_closed;		/* Check if the connection has to be closed */
		bool	_is_finished;	/* Check if the request has been completed */

		/* Request body */
		std::ofstream		_file;				/* File here the body info is saved */
		std::vector<char>	_body;				/* Body as vector */
		std::string			_boundary;			/* Body delimitier */
		std::string			_body_filename;		/* Provisional name of the file that has the body */
		std::string			_request_filename;	/* Name that the file should have */

		/* Process request aux */
		size_t	_content_length;	/* Body size */
		bool	_body_started;		/* Check if the body has started */
		bool	_body_headers_cleaned;	/* Check if the body headers (on files) are cleaned */

		/* Private aux functions */
		void	parse_headers(std::string headers);

	public:

		/* NOTE: Constructors and destructor */
		HTTPRequest( void );
		~HTTPRequest( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		std::string	get_method( void ) const;
		std::string	get_path( void ) const;
		std::string	get_protocol( void ) const;

		std::map<std::string, std::string>	get_headers( void ) const;
		std::pair<bool, std::string>		find_header( std::string header ) const;

		/* TODO: content getters */
		// std::string	get_content( void ) const;

		bool		check_closed( void ) const;
		bool		check_finished( void ) const;

		/* NOTE: Objects features */
		void	process_request( int fd );

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, HTTPRequest const& printObject );
