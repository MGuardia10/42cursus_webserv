#pragma once

# include <iostream>
# include "Server.hpp"
# include "HTTPRequest.hpp"

class Client
{
	private:
		int	const _id;					/* Client id */
		int const _fd;					/* Connection fd */
		Server const& _server;			/* Associated server */
		std::string const _cookie;		/* Session cookie */
		HTTPRequest *_current_request;	/* Last request */

		static int CLIENTS_COUNT;	/* Client generated, used on cookies*/

	public:
		/* NOTE: Constructors and destructor */
		Client( int fd, Server& server );
		~Client( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		int	get_fd( void ) const;
		Server const& get_server( void ) const;
		std::string get_cookie( void ) const;

		HTTPRequest*	get_request( void ) const;
		void			set_request( HTTPRequest* request );

		/* NOTE: Objects features */
		std::string print( void ) const;
		std::string	generate_cookie( void ) const;

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Client const& printObject );

