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

};
