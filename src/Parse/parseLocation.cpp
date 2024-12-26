#include "../../include/parse.hpp"

bool	is_location( const std::string line ) {

	/* Verificar que empieza por palabra location */
	if ( line.substr(0,9) != "location " ) return false;

	/* Verificar que hay palabra entre medias y que line acaba en { */
	size_t pos = line.rfind(' ');
	if ( pos == std::string::npos || pos == 8 || line.at( line.size() - 1 ) != '{' ) return false;

	/* Verificar que existe un nombre valido para la location */
	std::string name_location = line.substr(9, pos - 9);
	return !name_location.empty() && (!name_location.compare("/") || (name_location.at(0) == '/' && name_location.at( name_location.size() - 1 ) != '/'));
}

static bool check_duplicate_location( Server server, std::string name_location ) {

	/* Get server locations */
	std::map<std::string, Location>	locations = server.get_locations();

	/* Verify there is no duplicate locations */
	std::map<std::string, Location>::const_iterator it = locations.find( name_location );
	return !(it == locations.end());
}

int	parse_location( std::ifstream &file, std::string &line, Server &server, std::map<std::string, Function> location_directives ) {
	

	/* Get route of Location */
	std::string name_location = line.substr( 9, line.rfind(' ') - 9 );

	/* Verify duplicate Locations */
	if ( check_duplicate_location( server, name_location ) )
		return 0;

	/* Create new Location */
	Location location( name_location );

	/* Read lines inside a location */
	while (std::getline( file, line )) {

		/* Normalize string */
		normalize_string( line );

		/* Case end of Location */
		if ( line.compare("}") == 0 ) {

			server.add_location( name_location, location );
			break;
		}
		/* Case empty line or comment */
		else if ( line.empty() || line.at(0) == '#' ) continue ;
		/* Case valid directive on Location */
		else if ( is_valid_directive( line ) && add_directive( line, server, location_directives ) ) continue;
		/* Case error */
		else return 0;
	}

	return 1;

}