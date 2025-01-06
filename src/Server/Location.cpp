#include "../../include/Location.hpp"

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

Location::Location( void ) : ConfigBase(),
	_route(""),
	_alias(ALIAS_DEFAULT)
{}

Location::Location( std::string const& route ) : ConfigBase(),
	_route(route),
	_alias(ALIAS_DEFAULT)
{}

Location::~Location( void )
{
	/* TODO */
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/

std::string		Location::print( void ) const
{
	std::string	buffer = "";

	buffer += "\t[ LOCATION ] " + _route + "\n";
	buffer += "\t\t· Alias: \"" + _alias + "\"\n";
	buffer += static_cast<ConfigBase const&>(*this).print("\t");
	return buffer;
}

std::ostream&	operator<<( std::ostream& os, Location const& printObject )
{
	os << printObject.print();
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

std::string	Location::get_route( void ) const { return _route; }
void		Location::set_route( std::string route ) { _route = route; }

std::string	Location::get_alias( void ) const { return _alias; }
void		Location::set_alias( std::string alias ) { _alias = alias; }

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
