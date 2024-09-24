#pragma once

#include <iostream>
#include "ConfigBase.hpp"

class Location : public ConfigBase
{
	private:
		std::string	alias;

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
