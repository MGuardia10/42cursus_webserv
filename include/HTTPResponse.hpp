#pragma once

#include <iostream>
#include <map>
#include "default.hpp"

class HTTPResponse
{
	private:
		static std::map<int, std::string>	_errors;
		static std::map<std::string, std::string> _extensions;

		HTTPResponse( void );

		/**
		 * @brief Function to get all the basic/general headers.
		 * 
		 * @param	cookie Cookie of the client
		 * @param	end Flag to set the header end delimiter
		 * 
		 * @return The start header
		 */
		static std::string get_default_headers( int code, std::string cookie, bool end=true );

	protected:
	public:

		/* NOTE: Constructors and destructor */
		~HTTPResponse( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */

		/* NOTE: Objects features */

			/* NOTE: loads */
		/**
		 * @brief Function to load the errors types. It reads the info on the
		 * 			ERROR_PATH file.
		 */
		static bool	load_errors( void );

		/**
		 * @brief Function to load the errors types. It reads the info on the
		 * 			EXTENSIONS_PATH file.
		 */
		static bool	load_extensions( void );

			/* NOTE: pages */
		static std::string	get_response_template( int code, std::string msg, std::string cookie );


		/* NOTE: Exceptions */

};
