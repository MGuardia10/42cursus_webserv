template<typename T>
int add_directive( std::string const line, T &item, std::map<std::string, Function> const directives_map ) {

	/* Substr form start to first space on line */
	std::string key = line.substr( 0, line.find( ' ' ) + 1);

	/* Find directive on directives map */
	std::map<std::string, Function>::const_iterator it = directives_map.find( key );
	if ( it == directives_map.end() ) {
		normalize_string( key );
		throw std::invalid_argument("Error while parsing configuration file. Invalid directive name [" + key + "]." );
	}

	/* Create nwe string without key and normalize it */
	std::string values = line.substr( key.size(), line.size() - key.size() - 1 );
	normalize_string( values );

	/* Execute function associate with the key found */
	it->second( values , item );

	return 1;
}
