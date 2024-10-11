#include "../../include/Server.hpp"
#include <sstream>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

int Server::servers_count = 0;

Server::Server( void ) : ConfigBase(),
	_is_running(false),
	_ip(IP_DEFAULT),
	_port(-1)
{
	/* Set the name to the server */
	std::stringstream	ss;
	ss << ++Server::servers_count;
	_server_name = "Server " + ss.str();

	/* Clear the lists/vectors */
	_locations.clear();
}

Server::~Server( void )
{
	if (_is_running)
		this->stop();
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

	buffer += "[ SERVER ] " + _server_name;

	buffer += "\t· Is running: " + (_is_running ? std::string("true") : std::string("false"));

	ss << _port;
	buffer += "\t· Connection: " + _ip + ":" + ss.str() + "\n";

	buffer += "\t· Locations:\n";
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
int		Server::get_port( void ) const { return _port; }
void	Server::set_port( int port ) { _port = port; }

/* Server name */
std::string	Server::get_server_name( void ) const { return _server_name; }
void		Server::set_server_name( std::string server_name ) { _server_name = server_name; }

/* Location */
std::map<std::string, Location>	Server::get_locations( void ) const { return _locations; }

Location const*	Server::get_location( std::string name ) const
{
	std::map<std::string, Location>::const_iterator it = _locations.find(name);
	return (it == _locations.end() ? NULL : &(it->second));
}

void	Server::add_location( std::string name, Location location )
{
	if (_locations.find( name ) == _locations.end())
		_locations.insert(std::pair<std::string, Location>(name, location));
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

void	Server::run( void )
{
	if (_is_running)
		return ;
	_is_running = true;

	/* TODO: Implementation of the function */
}

void	Server::stop( void )
{
	if (!_is_running)
		return ;
	_is_running = false;

	/* TODO: Implementation of the function */
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
