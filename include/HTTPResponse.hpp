#pragma once

#include <iostream>

class HTTPResponse
{
	private:
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
		static std::string	get_response_template( int code, std::string msg, std::string cookie );


		/* NOTE: Exceptions */

};
