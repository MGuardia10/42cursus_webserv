#pragma once

#include <iostream>
#include <vector>
#include "ConfigBase.hpp"
#include "Location.hpp"
#include "default.hpp"

class Server : public ConfigBase
{
	private:
		/* Number of servers created */
		static int	servers_count;

		/* Flag to know if the server is running */
		bool	_is_running;

		/* Port where the server will be listening */
		std::string	_ip;
		int			_port;

		/* Name of the server */
		std::string	_server_name;

		/* List of the server locations */
		std::vector<Location>	_locations;

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
