#include "../../include/Server.hpp"
#include <sstream>
#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

int Server::servers_count = 0;

Server::Server( void ) : ConfigBase(),
	_is_running(false),
	_ip(IP_DEFAULT)
{
	/* Set the name to the server */
	std::stringstream	ss;
	ss << ++Server::servers_count;
	_server_name = "Server " + ss.str();

	_ports.push_back(-1);
	_sockets.clear();

	/* Clear the lists/vectors */
	_locations.clear();
}

Server::~Server( void )
{
	_is_running = false;
	_locations.clear();
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/

std::string	Server::print( void ) const
{
	std::stringstream	ss;
	std::string			buffer = "";

	buffer += "[ SERVER ] " + _server_name + "\n";

	buffer += "\t· Address: " + _ip + "\n";

	buffer += "\t· Is running: " + (_is_running ? std::string("true") : std::string("false")) + "\n";

	buffer += "\t· Connections: ";
	if (_ports.size() == 0)
		buffer += "None";
	buffer += "\n";
	for (std::vector<int>::const_iterator it = _ports.begin(); it != _ports.end(); it++)
	{
		ss.str("");
		ss << *it;
		buffer += "\t\t- " + ss.str() + "\n";
	}

	buffer += static_cast<ConfigBase const&>(*this).print();

	buffer += "\t· Locations:\n\n";
	for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
		buffer += it->second.print() + "\n";

	return buffer;
}

std::ostream&	operator<<( std::ostream& os, Server const& printObject )
{
	os << printObject.print();
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

int	Server::is_running( void ) const { return _is_running; }

/* IP */
std::string	Server::get_ip( void ) const { return _ip; }
void		Server::set_ip( std::string ip ) { _ip = ip; }

/* Port */
std::vector<int>	Server::get_ports( void ) const { return _ports; }
void	Server::add_port( int port )
{
	if (_ports.size() == 1 && _ports.at(0) == -1)
		_ports.clear();
	if (!has_port(port))
		_ports.push_back(port);
}
bool	Server::has_port( int port )
{
	return std::find(_ports.begin(), _ports.end(), port) != _ports.end();
}

/* Server name */
std::string	Server::get_server_name( void ) const { return _server_name; }
void		Server::set_server_name( std::string server_name ) { _server_name = server_name; }

/* Sockets */
std::vector<int>	Server::get_sockets( void ) const { return _sockets; }
bool				Server::has_socket( int socket_fd ) const
{
	return std::find(_sockets.begin(), _sockets.end(), socket_fd) != _sockets.end();
}

/* Location */
std::map<std::string, Location>	Server::get_locations( void ) const { return _locations; }
std::pair<bool, Location const*>	Server::get_location( std::string route ) const
{
	std::map<std::string, Location>::const_iterator it;
	int index;

	/* Check if the route exists on the saved locations maps */
	std::string last;
	while (1)
	{
		it = _locations.find(route);
		if (it != _locations.end())
			return std::pair<bool, Location const*>(true, &(it->second));

		index = route.rfind("/");
		last = route.substr(0, index);
		if (last.empty())
			last = "/";
		if (last == route)
			break ;
		route = last;
	};

	/* The location has not been found */
	return std::pair<bool, Location const*>(false, NULL);
}

void	Server::add_location( std::string route, Location location )
{
	if (_locations.find( route ) == _locations.end())
	{
		location.inherit(*this);
		_locations.insert(std::pair<std::string, Location>(route, location));
	}
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

void	Server::run( void )
{
	/* Check if the server is running */
	if (_is_running)
		return ;
	_is_running = true;

	/* Create all the ports connections */
	for (std::vector<int>::iterator it = _ports.begin(); it != _ports.end(); it++)
	{
		int port = *it;

		if (port < 0 && port > 65535)
			throw ServerException("Invalid port");

		/* Open the socket */
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1)
			throw ServerException("Error while initializing the socket", strerror(errno));
		
		/* Set the socket to non-blocking */
		int flags = fcntl(socket_fd, F_GETFL, 0);
		if (flags == -1 || fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			close(socket_fd);
			throw ServerException("Error setting the socket to non-blocking", strerror(errno));
		}

		/* Configure the socket */
		int optval = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			close(socket_fd);
			throw ServerException("Error setting the socket configuration", strerror(errno));
		}

		/* Bind the socket to the specefied IP and port */
		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr) <= 0)
		{
			close(socket_fd);
			throw ServerException("Invalid server IP", strerror(errno));
		}

		/* Bind the socket to the port */
		if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		{
			close(socket_fd);
			throw ServerException("Error binding the socket", strerror(errno));
		}

		/* Active the listen */
		if (listen(socket_fd, BACKLOG) == -1)
		{
			close(socket_fd);
			throw ServerException("Error while starting the listen", strerror(errno));
		}

		/* Add the socket to the active sockets list */
		_sockets.push_back(socket_fd);
	}
}

void	Server::stop( void )
{
	/* Check if the server is running */
	if (!_is_running)
		return ;
	_is_running = false;

	/* Close all the socket fds */
	for (std::vector<int>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
		close(*it);
}

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

Server::ServerException::ServerException( std::string const msg ) throw():
	_msg(msg)
{}

Server::ServerException::ServerException( std::string const msg, std::string const error ) throw():
	_msg(msg + ": " + error)
{}

Server::ServerException::~ServerException( void ) throw() {}

const char* Server::ServerException::what( void ) const throw()
{
	return _msg.c_str();
}

/*==========*/
/* !SECTION */
/*==========*/
