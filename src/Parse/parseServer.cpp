#include "../../include/parse.hpp"

bool	is_valid_directive( const std::string line ) {
	/* Verify line finish with ";" */
	if ( line.empty() || line.at( line.size() - 1 ) != ';' ) return false;
	
	/* Verify only one ";" and quotes are closed OK */
	int quoteCount = 0;
	int semiColonCount = 0;

	for (size_t i = 0; i < line.size(); ++i) {
		if ( line[i] == '"' ) ++quoteCount;
		if ( line[i] == ';' && quoteCount % 2 == 0 ) ++semiColonCount;
	}

	return quoteCount % 2 == 0 && semiColonCount == 1;
}

void	parse_server( std::vector<Server> &servers, std::ifstream &file, std::string &line, std::map<std::string, Function> server_directives, std::map<std::string, Function> location_directives, bool &line_after_server ) {
	
	/* Declare a new Server */
	Server server;
	line_after_server = false;

	while (1) {

		/* Read a new line and normalize it */
		std::getline(file, line);
		normalize_string(line);

		/* Case end of a Server */
		if ( line.compare("}") == 0 ) {

			servers.push_back(server);

			if ( std::getline(file, line) ) {
				normalize_string( line );
				line_after_server = true;
			}

			return ;
		}

		/* Case EOF without reaching end bracket */
		if ( file.eof() )
			throw std::invalid_argument("Error while parsing configuration file. Check brackets are properly closed");
		
		/* Case empty line or comment in a Server */
		if ( line.empty() || line.at(0) == '#' ) continue ;

		/* Case Server directive found */
		if ( is_valid_directive( line ) && add_directive( line, server, server_directives )) continue ;
		/* Caso Location in a Server found */
		else if ( is_location( line ) && parse_location( file, line, server, location_directives ) ) continue ;
		/* Case Syntax error found */
		else
			throw std::invalid_argument("Error while parsing configuration file. Syntax error.");
	}
}
