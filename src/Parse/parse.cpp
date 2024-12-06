#include "../../include/parse.hpp"
/* FIXME: servers harcoded so the implementations can be done while the parse is not completed */ 
// static std::vector<Server> hardcodedServers( void )
// {
// 	std::vector<Server>	servers;

// 	/* TODO: Server 1 */
// 	Server s1;

// 	s1.set_ip("127.0.0.1");
// 	s1.set_port(8080);
// 	s1.set_server_name("Test");

// 	s1.add_error_page(400, "/400.html");
// 	s1.add_error_page(404, "/404.html");

// 	s1.set_client_max_size(1000);

// 	s1.add_index("index.html");
// 	s1.set_autoindex(false);

// 	s1.add_cgi("sh", "/bin/bash");
// 	s1.add_cgi("py", "/bin/python3");

// 	ConfigBase::ReturnData s1_return = {.code = 300, .text = "Redirigiendo..."};
// 	s1.set_return(s1_return);

// 	Location l1s1 = Location("/");
// 	ConfigBase::ReturnData l1s1_return = {.code = 200, .text = "Acceso a location /"};
// 	l1s1.set_return(l1s1_return);
// 	s1.add_location(l1s1.get_route(), l1s1);

// 	Location l2s1 = Location("/test");
// 	l2s1.add_cgi("py", "/bin/python");
// 	l2s1.set_alias("port.html");
// 	l2s1.set_client_max_size(123);
// 	s1.add_location(l2s1.get_route(), l2s1);

// 	servers.push_back(s1);

// 	/* TODO: Server 2*/
// 	Server s2;

// 	s2.set_ip("127.0.0.1");
// 	s2.set_port(8081);
// 	s2.set_server_name("Test2");
// 	s2.set_root("/");

// 	s2.set_client_max_size(1000);

// 	Location l1s2 = Location("/");
// 	l1s2.set_root("/pages");
// 	l1s2.add_index("index.html");
// 	s2.add_location(l1s2.get_route(), l1s2);

// 	Location l2s2 = Location("/redirect");
// 	ConfigBase::ReturnData l2s2_return = {
// 		.code = 300,
// 		.text = "https://www.google.com"
// 	};
// 	l2s2.set_return(l2s2_return);
// 	s2.add_location(l2s2.get_route(), l2s2);

// 	servers.push_back(s2);

// 	return servers;
// }

void normalizeString(std::string& str) {
    std::string result;
    bool inSpace = false;

    for (size_t i = 0; i < str.size(); ++i) {
        if (std::isspace(str[i])) {
            if (!inSpace) {
                // Añade un único espacio si no estabas en una secuencia de espacios
                result += ' ';
                inSpace = true;
            }
        } else {
            // Añade caracteres no espacio y reinicia el estado
            result += str[i];
            inSpace = false;
        }
    }

    // Eliminar espacios al principio y al final
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

    // Comprobar si el archivo existe y es accesible (? no me funcionan los permisos debo ser superuser)

	// struct stat fileInfo; // #include <sys/stat.h>
    // if (stat(filename.c_str(), &fileInfo) != 0) {
    //     std::cerr << "Error: No se puede acceder al archivo o no existe." << std::endl;
    //     return servers;
    // }

	if ( !file.is_open() ) {
		std::cerr << "Error opening file: " << filename << '\n';
        return servers;
	}

	std::string line;
    while (std::getline(file, line)) {

		/* Normalizar espacios/tabulaciones entre palabras, al principio y al final de la linea */
		normalizeString( line );

		/* Saltar siguiente linea si la linea ha quedado vacia o es comentario */
		if ( line.empty() || line.at(0) == '#' )
			continue ;

		/* Parseo de servidores */
		if (line.compare("server {") == 0) {

			/* Verificar sintaxis separando por servers, actualizando file!!!!! */
			while (line.compare("server {") == 0) {
				/* Llegados a este punto estamos dentro de un server */
				std::string buffer = "";

				/* 1. Checkear sintaxis guardando archivo en un buffer linea a linea, hasta llegar a EOF o "server {" */
				/* 2. Si sintaxis OK, proceder a creación de server. Si sintaxis KO, limpiar servers y devolver servers vacio */
				if ( valid_sintax(file, line, buffer) ) {
					servers.push_back( create_server( buffer ) );
					buffer.clear();
				}
				else if ( file.eof() ) // comprobar este caso, no estoy seguro de como funciona eof
					continue ;
				else {
					servers.clear();
					return servers;
				}


			}
		}
		else {
			servers.clear();
			return servers;
		}

        std::cout << line << std::endl;
    }

	/* Close file */
	file.close();

	/* Return servers vector containing all servers added on config file */
	return servers;
	// return hardcodedServers();
}
