#include "../../include/CGIClient.hpp"
#include "../../include/default.hpp"
#include <unistd.h>
#include <sstream>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

CGIClient::CGIClient( Client& client, Location* location, pid_t pid, int pipe_fd, HTTPRequest* request ) :
	_client(client),
	_location(location),
	_pid(pid),
	_pipe_fd(pipe_fd),
	_request(request),
	_data("")
{
	_start_time = time(NULL);
}

CGIClient::~CGIClient( void )
{
	close(_pipe_fd);

	_request->remove_body_file();
	delete _request;
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/

std::string	CGIClient::print( void ) const
{
	std::string buffer = "";
	std::stringstream	ss;

	ss.str("");
	ss << _client.get_fd();
	buffer += "[CGIClient] Client: " + ss.str() + "\n";
	
	ss.str("");
	ss << _pid;
	buffer += "\t- PID: " + ss.str() + "\n";

	ss.str("");
	ss << _pipe_fd;
	buffer += "\t- Pipe fd: " + ss.str() + "\n";

	ss.str("");
	ss << _start_time;
	buffer += "\t- Start time: " + ss.str() + "\n";

	return buffer;
}

std::ostream&	operator<<( std::ostream& os, CGIClient const& printObject )
{
	os << printObject.print();
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

Client&			CGIClient::get_client( void ) const { return _client; }
Location*		CGIClient::get_location( void ) const { return _location; }
pid_t			CGIClient::get_pid( void ) const { return _pid; }
int				CGIClient::get_pipe_fd( void ) const { return _pipe_fd; }
time_t			CGIClient::get_start_time( void ) const { return _start_time; }
HTTPRequest*	CGIClient::get_request( void ) const { return _request; }

std::string		CGIClient::get_data( void ) const { return _data; }
void			CGIClient::add_data( std::string data ) { _data += data; }

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

bool	CGIClient::has_timeout( void ) const
{
	time_t	current = time(NULL);

	return current - _start_time > static_cast<time_t>(CGI_TIMEOUT);
}

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
