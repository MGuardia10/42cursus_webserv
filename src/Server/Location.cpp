#include "../../include/Location.hpp"

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

Location::Location( void ) : ConfigBase(),
	_route(""),
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
std::ostream&	operator<<( std::ostream& os, Location const& printObject )
{
	/* TODO */
	(void)printObject;
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

std::string	Location::get_route( void ) const { return _route; }
void		Location::set_route( std::string route ) { _route = route; }

std::string	Location::get_alias( void ) { return _alias; }
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
