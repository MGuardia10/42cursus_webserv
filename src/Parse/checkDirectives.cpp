#include "../../include/parse.hpp"

bool	is_valid_ipv4( std::string line ) {

	std::istringstream stream( line );
    std::string segment;
    std::vector<std::string> octets;

    /* Split line by dots */
    while ( std::getline( stream, segment, '.' ) )
        octets.push_back( segment );

    /* A valid IPv4 must have 4 octets */
    if ( octets.size() != 4 )
		return false;

    /* Verify each octet */
    for ( size_t i = 0; i < octets.size(); ++i ) {

        const std::string& octet = octets[i];

        if ( octet.empty() || octet.find_first_not_of( "0123456789" ) != std::string::npos )
            return false;

        char *endptr;
        long num = strtol( octet.c_str(), &endptr, 10 );

        if ( *endptr != '\0' || num < 0 || num > 255 )
            return false;
    }

	return true;
}

bool	is_valid_port( std::string line ) {

    /* Case line is empty */
    if ( line.empty() )
        return false;

    /* Get integer from string */
    char* endptr;
    long port = strtol( line.c_str(), &endptr, 10 );

    /* Return is port between 0 and 65535 */
    return !( *endptr != '\0' || port < 0 || port > 65535 );
}

bool    is_valid_absolute_path( std::string line ) {
    return line.at(0) == '/' && line.find( ' ' ) == std::string::npos;
}