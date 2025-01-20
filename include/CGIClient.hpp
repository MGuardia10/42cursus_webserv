#pragma once

#include <iostream>
#include "Client.hpp"
#include "HTTPRequest.hpp"

class CGIClient
{
	private:
		Client&			_client;
		Location*		_location;	
		pid_t			_pid;
		int				_pipe_fd;
		time_t			_start_time;
		HTTPRequest*	_request;

		std::string		_data;

	public:

		/* NOTE: Constructors and destructor */
		CGIClient( Client& client, Location* location, pid_t pid, int pipe_fd, HTTPRequest* request );
		~CGIClient( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		Client&			get_client( void ) const;
		Location*		get_location( void ) const;
		pid_t			get_pid( void ) const;
		int				get_pipe_fd( void ) const;
		time_t			get_start_time( void ) const;
		HTTPRequest*	get_request( void ) const;

		std::string		get_data( void ) const;
		void			add_data( std::string data );

		/* NOTE: Objects features */
		std::string print( void ) const;
		bool	has_timeout( void ) const;

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, CGIClient const& printObject );
