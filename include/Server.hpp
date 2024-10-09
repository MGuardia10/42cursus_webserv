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
		std::map<std::string, Location>	_locations;

	public:

		/* NOTE: Constructors and destructor */
		Server( void );
		~Server( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		int	is_running( void ) const;
		
		std::string	get_ip( void ) const;
		void		set_ip( std::string ip );

		int			get_port( void ) const;
		void		set_port( int port );

		std::string	get_server_name( void ) const;
		void		set_server_name( std::string server_name );

		std::map<std::string, Location>	get_locations( void ) const;
		Location const*					get_location( std::string name ) const;
		void							add_location( std::string name, Location location );

		/* NOTE: Objects features */
		std::string	print( void ) const;

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Server const& printObject );
