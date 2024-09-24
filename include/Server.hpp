#pragma once

#include <iostream>
#include <vector>
#include "ConfigBase.hpp"
#include "Location.hpp"

class Server : public ConfigBase
{
	private:
		/* Port where the server will be listening */
		int	port;

		/* Name of the server */
		std::string	server_name;

		/* List of the server locations */
		std::vector<Location>	locations;

	public:

		/* NOTE: Constructors and destructor */
		Server( void );
		Server( Server const& other );
		~Server( void );

		/* NOTE: Member operators overloading */
		/* Comparator operators */
		Server&	operator=( const Server &data );

		/* NOTE: Getters and setters */

		/* NOTE: Objects features */

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Server const& printObject );
