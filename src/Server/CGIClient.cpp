#include "../../include/CGIClient.hpp"
#include <unistd.h>
#include <sstream>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

CGIClient::CGIClient( Client const& client, pid_t pid, int pipe_fd ) :
	_client(client),
	_pid(pid),
	_pipe_fd(pipe_fd)
{
	_start_time = clock();
}

CGIClient::~CGIClient( void )
{
	close(_pipe_fd);
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

Client const&	CGIClient::get_client( void ) const { return _client; }
pid_t			CGIClient::get_pid( void ) const { return _pid; }
int				CGIClient::get_pipe_fd( void ) const { return _pipe_fd; }
time_t			CGIClient::get_start_time( void ) const { return _start_time; }

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
