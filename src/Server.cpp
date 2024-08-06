#include "Server.hpp"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
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

/*============================================================================*/

static std::string	findFile( char const* buffer )
{
	int			i = 0;
	std::string	file = "";

	/* Skip the first word and spaces */
	while (buffer[i] && buffer[i] != ' ')
		i++;
	while (buffer[i] && buffer[i] == ' ')
		i++;
	
	/* Get the second word */
	while (buffer[i] && buffer[i] != ' ')
	{
		file += buffer[i];
		i++;
	}

	return (file);
}

static std::string	readfile( std::string const& path )
{
	std::string		buffer;
	std::string		content = "";
	std::ifstream	file;

	file.open(path.c_str());
	if (!file)
		return ("");
	
	while (std::getline(file, buffer))
		content += buffer;

	file.close();
	return (content);
}

/* DEBUGGING: Print msg to know the request pages and the return ones */
static std::string	getFilePath( char const* buffer )
{
	struct stat	pathInfo;	
	std::string	path;
	int	res;

	/* Get the path */
	path = "pages" + findFile( buffer );
	std::cout << "{[" << path << "] => ";

	/* Check if the path is a folder or a file that exists */
	res = stat(path.c_str(), &pathInfo);
	if (res == -1 || (pathInfo.st_mode & S_IFMT) == S_IFDIR || access(path.c_str(), R_OK) == -1)
		path = "pages/error.html";
	std::cout << "[" << path << "]}" << std::endl;

	return (path);
}

static void	sendResponse( int connection, std::string const& content )
{
	std::stringstream	ss;
	std::string response;

	response =
		"HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n";
	
	ss << content.size();
	response += "Content-Length: " + ss.str() + "\r\n\r\n" + content;

	if (send(connection, response.c_str(), response.size(), 0) == -1)
	{
		close(connection);
		throw Server::ServerException("Error while sending data to a connection", strerror(errno));
	}
}

void	Server::acceptConnection( void )
{
	int			connection;
	char		buffer[1025] = {0};
	std::string	file, fileContent;

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

	/* Search the file of the request */
	file = getFilePath( buffer );

	/* Read file and send a message */
	sendResponse( connection, readfile(file) );

	/* Close the connection */
	close(connection);
}

/*============================================================================*/

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
