#pragma once

#include <iostream>
#include <map>
#include "default.hpp"

class HTTPResponse
{
	private:
		static std::map<int, std::string>	_codes;
		static std::map<std::string, std::string> _extensions;

		HTTPResponse( void );

		/**
		 * @brief Function to load the errors types. It reads the info on the
		 * 			ERROR_PATH file.
		 */
		static bool	load_codes( void );

		/**
		 * @brief Function to load the errors types. It reads the info on the
		 * 			EXTENSIONS_PATH file.
		 */
		static bool	load_extensions( void );

		/**
		 * @brief Function to get all the basic/general headers.
		 * 
		 * @param	cookie Cookie of the client
		 * @param	end Flag to set the header end delimiter
		 * 
		 * @return The start header
		 */
		static std::string get_default_headers( int code, std::string cookie, bool connection_alive=true, bool end=true );

	protected:
	public:

		/* NOTE: Constructors and destructor */
		static size_t const	CHUNK_SIZE = 1024;
		~HTTPResponse( void );

		/* NOTE: Loads */

		/**
		 * @brief Function to load all the data necessary to prepare the
		 * 			responses
		 */
		static bool	load_data( void );

		/* NOTE: pages */

		/**
		 * @brief Function to generate a general response, with a specified code and msg. Also,
		 * 			a cookie can be added
		 * 
		 * @param	code Response code
		 * @param	msg Message that will be displayed as HTML
		 * @param	cookie Cookie of the client. If it is empty, no cookie is added
		 * 
		 * @return A correct response that can be sent
		 */
		static std::string	get_response_template( int code, std::string msg, std::string cookie );

		/**
		 * @brief Function that generates a response that indicates that the connection is closed
		 * 
		 * @param	cookie Cookie of the user to whom the response will be sent. Can be empty
		 * 
		 * @return A correct response that can be sent
		 */
		static std::string	get_close_connection_template( std::string cookie );

		/**
		 * @brief Function to return the data of a file. If the file on `path` is not found, a general
		 * 			404 response is returned
		 * 
		 * @param	path Path of the file to send
		 * @param	cookie Cookie of the client
		 * @param	offset Offset on the file search. If it is 0, the header is generated, and the read begins;
		 * 					if it is different from 0, the reading continues from this value
		 * 
		 * @return The return has 2 fields: the first one indicates the last byte sent, if it is 0,
		 * 			the response returned is the last one; the second field is the response to sent.
		 * 			If the first value is negative, an error ocurred
		 */
		static std::pair<long long, std::string>	get_file_response( std::string path, std::string cookie, long long offset );
};
