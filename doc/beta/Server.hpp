#pragma once

#include <iostream>

class Server
{
	private:
		std::string	_name;
		int			_port;
		int			_backlog;
		bool		_active;

		int			_socket_fd;

		static int const	DEFAULT_BACKLOG = 10;

		void	initServer( void );

	public:

		/* NOTE: Constructors and destructor */
		Server( std::string const& name, int port );
		Server( std::string const& name, int port, int backlog );
		~Server( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */

		/* NOTE: Objects features */
		bool	start( void );
		bool	stop( void );
		void	acceptConnection( void );


		std::string	print( void ) const;

		/* NOTE: Exceptions */
		class ServerException : public std::exception
		{
			private:
				std::string	_msg;
		
			public:
				ServerException( std::string const& msg, std::string const& error ) throw();
				~ServerException( void ) throw();
				virtual char const* what() const throw();
		};

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Server const& printObject );
