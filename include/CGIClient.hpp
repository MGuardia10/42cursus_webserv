#pragma once

#include <iostream>
#include "Client.hpp"

class CGIClient
{
	private:
		Client const&	_client;
		pid_t			_pid;
		int				_pipe_fd;
		time_t			_start_time;

	public:

		/* NOTE: Constructors and destructor */
		CGIClient( Client const& client, pid_t pid, int pipe_fd );
		~CGIClient( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		Client const&	get_client( void ) const;
		pid_t			get_pid( void ) const;
		int				get_pipe_fd( void ) const;
		time_t			get_start_time( void ) const;

		/* NOTE: Objects features */
		std::string print( void ) const;

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, CGIClient const& printObject );
