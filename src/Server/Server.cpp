#include "../../include/Server.hpp"
#include <sstream>
#include <algorithm>

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
	{
		location.inherit(*this);
		_locations.insert(std::pair<std::string, Location>(name, location));
	}
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

Server::ServerException::ServerException( std::string const msg ) throw():
	_msg(msg)
{}

Server::ServerException::~ServerException( void ) throw() {}

const char* Server::ServerException::what( void ) const throw()
{
	return _msg.c_str();
}

/*==========*/
/* !SECTION */
/*==========*/
