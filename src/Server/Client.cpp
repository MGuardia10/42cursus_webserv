#include "../../include/Client.hpp"
#include <sstream>
#include <ctime>
#include <string.h>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

int Client::CLIENTS_COUNT = 1;

Client::Client( int fd, Server& server ):
	_id(Client::CLIENTS_COUNT),
	_fd(fd),
	_server(server),
	_cookie(generate_cookie())
{
	/* TODO: set the cookie */
	Client::CLIENTS_COUNT++;
}

Client::~Client( void )
{}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/

std::string Client::print( void ) const
{
	std::stringstream	ss;
	std::string buffer = "";

	ss << _id;
	buffer += "[ CLIENT ] ID: " + ss.str() + "\n";
	ss.str("");
	ss << _fd;
	buffer += "\t- Connection fd: " + ss.str() + "\n";
	buffer += "\t- Associated server: " + _server.get_server_name() + "\n";
	buffer += "\t- Cookie: " + _cookie + "\n";

	return buffer;
}

std::ostream&	operator<<( std::ostream& os, Client const& printObject )
{
	os << printObject.print();
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

int Client::get_fd( void ) const { return _fd; }
Server const& Client::get_server( void ) const { return _server; }
std::string Client::get_cookie( void ) const { return _cookie; }

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

/* /Set-Cookie:/ <nombre>=<valor>; Expires=<fecha>; Path=<ruta>; HttpOnly; Secure */
std::string Client::generate_cookie( void ) const
{
	std::string			cookie = "";

	/* Session cookie  */
	std::stringstream	ss;
	ss << _id;
	cookie += "sessionid=" + ss.str() + "; ";

	/* Expires */
	time_t now_long = time(NULL);
	tm *now_struct = gmtime(&now_long);
	now_struct->tm_mday += 1;
	char now_string[80] = {0};
	strftime(now_string, 80, "%a, %d-%b-%Y %H:%M:%S GMT", now_struct);
	cookie += "Expires=" + std::string(now_string) + "; ";

	/* Path */
	cookie += "Path=/; ";

	/* Rest of header options */
	cookie += "HttpOnly";

	return (cookie);
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
