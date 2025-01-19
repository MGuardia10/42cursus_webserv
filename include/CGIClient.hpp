#pragma once

#include <iostream>
#include "Client.hpp"
#include "HTTPRequest.hpp"

class CGIClient
{
	private:
		Client const&	_client;
		pid_t			_pid;
		int				_pipe_fd;
		time_t			_start_time;
		HTTPRequest*	_request;

	public:

		/* NOTE: Constructors and destructor */
		CGIClient( Client const& client, pid_t pid, int pipe_fd, HTTPRequest* request );
		~CGIClient( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		Client const&	get_client( void ) const;
		pid_t			get_pid( void ) const;
		int				get_pipe_fd( void ) const;
		time_t			get_start_time( void ) const;
		HTTPRequest*	get_request( void ) const;

		/* NOTE: Objects features */
		std::string print( void ) const;
		bool	has_timeout( void ) const;

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, CGIClient const& printObject );
