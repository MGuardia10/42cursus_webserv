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

Server::Server( Server const& other ) : ConfigBase( other )
{
	/* TODO */
	(void)other;
}

Server::~Server( void )
{
	/* TODO */
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/**********************************/
/* NOTE: Copy assignment operator */
/**********************************/
Server&	Server::operator=( Server const& other )
{
	if (this != &other)
	{
		/* TODO */
	}
	return (*this);
}

/***********************/
/* NOTE: '<<' operator */
/***********************/
std::ostream&	operator<<( std::ostream& os, Server const& printObject )
{
	/* TODO */
	(void) printObject;
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

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
