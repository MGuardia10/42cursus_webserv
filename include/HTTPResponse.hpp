#pragma once

#include <iostream>
#include <map>
#include "default.hpp"
#include "Server.hpp"

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
		 * @param	code Response code, the message will be getted by the `CODES_PATH` file
		 * @param	cookie Cookie of the client
		 * @param	connection_alive Check if the connection must be closed or not
		 * 
		 * @return A map with the defaults headers
		 */
		static std::map<std::string, std::string> get_default_headers( int code, std::string cookie, bool connection_alive=true );

		/**
		 * @brief Function to generate a general response, with a specified code and msg. Also,
		 * 			a cookie can be added
		 * 
		 * @param	code Response code
		 * @param	msg Message that will be displayed as HTML. If there is no msg, the default
		 * 				error type is displayed
		 * @param	cookie Cookie of the client. If it is empty, no cookie is added
		 * 
		 * @return A correct response that can be sent
		 */
		static std::string	get_response_template( int code, std::string msg, std::string cookie );

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
		 * @param	code Code that the response will have. If a negative number is provided, the code
		 * 					will be 200
		 * @param	path Path of the file to send
		 * @param	cookie Cookie of the client
		 * @param	offset Offset on the file search. If it is 0, the header is generated, and the read begins;
		 * 					if it is different from 0, the reading continues from this value
		 * 
		 * @return The return has 2 fields: the first one indicates the last byte sent, if it is 0,
		 * 			the response returned is the last one; the second field is the response to sent.
		 * 			If the first value is negative, an error ocurred
		 */
		static std::pair<long long, std::string>	get_file_response( int code, std::string path, std::string cookie, long long offset );

		/**
		 * @brief Function to generate the response of a return statement
		 * 
		 * @param	data Information about the return
		 * @param	cookie User client
		 * 
		 * @return The response created
		 */
		static std::string	get_return_response( ConfigBase::ReturnData* data, std::string cookie );

		/**
		 * @brief Function to return an error page
		 * 
		 * @param	code Code that the response will have
		 * @param	path Path where the error page is saved. If path is empty (""), a default page
		 * 					is generated
		 * @param	cookie Cookie of the client
		 * @param	offset index of the last bytes sent; used if the page is too long
		 * 
		 * @return	On the first field, the index of the last byte sent: if it is 0, the file has been
		 * 				read; on the second field, the current response is formed
		 */
		static std::pair<long long, std::string>	get_error_page_response( int code, std::string path, std::string cookie, long long offset );

		/**
		 * @brief Function to generate the autoindex of a specific path
		 * 
		 * @param	path Path of the root folder
		 * @param	cookie Cookie of the client
		 * 
		 * @return The correct response
		 */
		static std::string	get_autoindex_response( std::string path, std::string request_path, std::string cookie );
};
