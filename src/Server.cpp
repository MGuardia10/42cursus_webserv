#include "Server.hpp"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

void	Server::initServer( void )
{
	/* Open the socket */
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
		throw ServerException("Error creating socket", strerror(errno));
	
	/* Configure the socket */
	int	optval = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
	{
		close(_socket_fd);
		throw ServerException("Error setting the socket configuration", strerror(errno));
	}

	/* Bind the port and the socket */
	struct sockaddr_in	addr = {
		.sin_family = AF_INET,
		.sin_port = htons(_port),
		.sin_addr = INADDR_ANY,
		.sin_zero = {0}
	};
	if (bind(_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		close(_socket_fd);
		throw ServerException("Error binding the socket to the port", strerror(errno));
	}

	/* Msg */
	std::cout << "[ " << _name << " ] Opening server." << std::endl;
}

Server::Server( std::string const& name, int port ) :
	_name(name),
	_port(port),
	_backlog(DEFAULT_BACKLOG),

	_active(false)
{
	initServer();
}

Server::Server( std::string const& name, int port, int backlog ) :
	_name(name),
	_port(port),
	_backlog(backlog),

	_active(false)
{
	initServer();
}

Server::~Server( void )
{
	std::cout << "[ " << _name << " ] Closing server." << std::endl;
	stop();
	if (_socket_fd > 0)
		close(_socket_fd);
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/
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

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

bool	Server::start( void )
{
	/* Check if the server is currently running */
	if (_active)
		return (false);
	_active = true;

	/* Listen the socket */
	if (listen(_socket_fd, _backlog) == -1)
		throw ServerException("Error while listening starts", strerror(errno));

	return (true);
}

bool	Server::stop( void )
{
	if (!_active)
		return (false);
	_active = false;
	return (true);
}

void	Server::acceptConnection( void )
{
	int		connection;
	char	buffer[1025] = {0};

	/* Wait for a connection */
	connection = accept(_socket_fd, NULL, NULL);
	if (connection == -1)
		return ;

	/* Recieve the msg */
	if (recv(connection, buffer, 1024, 0) == -1)
	{
		close(connection);
		throw ServerException("Error receiving data from connection", strerror(errno));
	}
	std::cout << "============================================" << std::endl;
	std::cout << "[" << _name << "] New connection" << std::endl;
	std::cout << "\t- Connection: " << connection << std::endl;
	std::cout << "\t- Data:" << std::endl << buffer << std::endl;

	/* Send a message */
	std::string response =
		"HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 81\r\n"
        "\r\n"
        "<html><head><title>Test</title></head><body><p>Test de webserv!</p></body></html>";
	char const*	response_c = response.c_str();
	if (send(connection, response_c, strlen(response_c), 0) == -1)
	{
		close(connection);
		throw ServerException("Error while sending data to a connection", strerror(errno));
	}

	/* Close the connection */
	close(connection);
}

std::string	Server::print( void ) const
{
	std::stringstream	ss;
	std::string			buffer = "";

	/* Name */
	buffer += "[ " + _name + " ]\n";

	/* Port */
	ss << _port;
	buffer += "\t- Port: " + ss.str();
	ss.str("");

	/* Socket fd */
	ss << _socket_fd;
	buffer += "\n\t- Associate fd: " + ss.str();
	ss.str("");

	/* Status of the server */
	buffer += "\n\t- Status: ";
	if (_active)	buffer += "Active";
	else			buffer += "Disabled";
	buffer += "\n";


	return (buffer);
}

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

Server::ServerException::ServerException( std::string const& msg, std::string const& error ) throw() :
	_msg(msg + ": " + error)
{}

Server::ServerException::~ServerException( void ) throw()
{}

char const*	Server::ServerException::what() const throw()
{
	return (_msg.c_str());
}

/*==========*/
/* !SECTION */
/*==========*/
