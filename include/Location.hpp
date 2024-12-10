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
		Location( std::string const& route );
		~Location( void );

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		std::string	get_route( void ) const;
		void		set_route( std::string route );

		std::string	get_alias( void );
		void		set_alias( std::string alias );


		/* NOTE: Objects features */
		std::string	print( void ) const;

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, Location const& printObject );
