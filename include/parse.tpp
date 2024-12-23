template<typename T>
int add_directive( std::string const line, T &item, std::map<std::string, Function> const directives_map ) {

	/* Substr form start to first space on line */
	std::string key = line.substr( 0, line.find( ' ' ) + 1);

	/* Find directive on directives map */
	std::map<std::string, Function>::const_iterator it = directives_map.find( key );
	if ( it == directives_map.end() ) return 0;

	/* Execute function associate with the key found */
	it->second( line, item );

	return 1;
}
