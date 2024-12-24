#include "../../include/parse.hpp"
#include "../../include/colors.hpp"

void normalize_string(std::string& str) {
    std::string result;
    bool inSpace = false;

	/* Remove spaces between words */
    for (size_t i = 0; i < str.size(); ++i) {
        if (std::isspace(str[i])) {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            result += str[i];
            inSpace = false;
        }
    }

    /* Remove spaces at start and end of line */
    size_t start = 0;
	size_t end = result.size();

    while (start < end && std::isspace(result[start])) ++start;
    while (end > start && std::isspace(result[end - 1])) --end;

    result = result.substr(start, end - start);
    str = result;
}

std::vector<Server>	parse( std::string const& filename )
{
	std::vector<Server> servers;
	std::ifstream file(filename.c_str());

	servers.clear();

	/* Check file is open */
	if ( !file.is_open() ) {
		std::cerr << "Error opening file: " << filename << '\n';
        return servers;
	}

	try
	{
		std::string line;
		bool line_after_server = true;

		/* Get server/location directives in a map */
		std::map<std::string, Function> server_directives = get_server_directives();
		std::map<std::string, Function> location_directives = get_location_directives();

		while (std::getline(file, line)) {

			/* Normalize spaces/tabs at start/end of line and between words */
			normalize_string( line );

			/* Case line is empty or is a comment */
			if ( line.empty() || line.at(0) == '#' )
				continue ;

			/* Case line is the start of a Server */
			while (line.compare("server {") == 0)
				parse_server( servers, file, line, server_directives, location_directives, line_after_server );

			/* Case lines between Servers */
			if ( !line.empty() && line_after_server ) {
				if ( line.at(0) != '#' )
					throw std::invalid_argument("Error while parsing configuration file.");
			}
		}
	}
	catch( const std::exception& e )
	{
		/* Clear servers vector */
		servers.clear();

		/* Print error on stderror */
		std::cerr << RED << "[ ERROR ] " << filename << ": " << e.what() << RESET << '\n';
	}
	
	/* Close config file */
	file.close();

	/* Return Servers Vector */
	return servers;
}
