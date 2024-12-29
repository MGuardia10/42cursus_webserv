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
		size_t	_content_length;		/* Body size */
		bool	_body_started;			/* Check if the body has started */
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

		bool		check_closed( void ) const;
		bool		check_finished( void ) const;
		std::string	get_filename( void ) const;

		/* NOTE: Objects features */

		/**
		 * @brief Function to get and process a HTML request
		 * 
		 * @param	fd File descriptor where the request is recieved
		 * 
		 * @return void
		 */
		void	process_request( int fd );

		/**
		 * @brief Function to move the body file to a specific path
		 * 
		 * @param	dest_path The destination path
		 * 
		 * @return void
		 */
		void	move_body_file(std::string const& dest);

		/**
		 * @brief Function to remove the body file
		 * 
		 * @return void
		 */
		void	remove_body_file( void );

		std::string	print( void ) const;

		/* NOTE: Exceptions */
		class HTTPRequestException : std::exception
		{
			private:
				std::string const _msg;
			public:
				HTTPRequestException( std::string msg ) throw();
				virtual ~HTTPRequestException( void ) throw();
				virtual const char* what( void ) const throw();
		};

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, HTTPRequest const& printObject );
