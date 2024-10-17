#include "../../include/parse.hpp"

/* FIXME: servers harcoded so the implementations can be done while the parse is not completed */ 
static std::vector<Server> hardcodedServers( void )
{
	std::vector<Server>	servers;

	/* TODO: Server 1 */
	/* TODO: Server 2*/

	return servers;
}

std::vector<Server>	parse( std::string const& filename )
{
	(void) filename;
	return hardcodedServers();
}
