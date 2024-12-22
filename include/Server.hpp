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
		std::string			_ip;
		std::vector<int>	_ports;

		/* Name of the server */
		std::string	_server_name;

		/* Sockets */
		std::vector<int>	_sockets;

		/* List of the server locations */
		std::map<std::string, Location>	_locations;

		static int const BACKLOG = 10;

	public:

		/* NOTE: Constructors and destructor */
		Server( void );
		~Server( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		int	is_running( void ) const;
		
		std::string	get_ip( void ) const;
		void		set_ip( std::string ip );

		std::vector<int>	get_ports( void ) const;
		void				add_port( int port );
		bool				has_port( int port );

		std::string	get_server_name( void ) const;
		void		set_server_name( std::string server_name );

		std::vector<int>	get_sockets( void ) const;
		bool				has_socket( int sckt ) const;

		std::map<std::string, Location>		get_locations( void ) const;
		std::pair<bool, Location const*>	get_location( std::string route ) const;
		void								add_location( std::string route, Location location );

		/* NOTE: Objects features */
		std::string	print( void ) const;

		void	run( void );
		void	stop( void );

		/* NOTE: Exceptions */
		class ServerException : std::exception
		{
			private:
				std::string _msg;
			public:
				ServerException( std::string const msg ) throw();
				ServerException( std::string const msg, std::string const error ) throw();
				virtual ~ServerException( void ) throw();
				virtual const char* what( void ) const throw();
		};

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Server const& printObject );
