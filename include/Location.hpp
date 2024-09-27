#pragma once

#include <iostream>
#include "ConfigBase.hpp"
#include "default.hpp"

class Location : public ConfigBase
{
	private:
		/* Route of the location */
		std::string	_route;

		/* Alias of the route */
		std::string	_alias;

	public:

		/* NOTE: Constructors and destructor */
		Location( void );
		Location( Location const& other );
		~Location( void );

		/* NOTE: Member operators overloading */
		/* Comparator operators */
		Location&	operator=( const Location &data );

		/* NOTE: Getters and setters */

		/* NOTE: Objects features */

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Location const& printObject );
