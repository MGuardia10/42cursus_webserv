#pragma once

#include <iostream>
#include <map>
#include <vector>

class ConfigBase
{
	public:
		struct ReturnData {
			int			code;
			std::string	text;
		};

	private:
		/* Route of the location */
		std::string	_route;

		/* Error pages: <status_code, page_path> */
		std::map<int, std::string>	_error_pages;

		/* Client max request size (vutes/characters) */
		size_t	_client_max_body_size;

		/* Root folder, where the files will be search */
		std::string	_root;

		/* Index files */
		std::vector<std::string>	_indexes;

		/* Autoindex (list the files on a folder) */
		bool	_autoindex;

		/* List of possible cgis */
		std::vector< std::map<std::string, std::string> >	_cgi;

		/* Return field */
		ReturnData	_returnData;

	public:


		/* NOTE: Constructors and destructor */
		ConfigBase( void );
		ConfigBase( ConfigBase const& other );
		~ConfigBase( void );

		/* NOTE: Member operators overloading */
		/* Comparator operators */
		ConfigBase&	operator=( const ConfigBase &data );

		/* NOTE: Getters and setters */

		/* NOTE: Objects features */

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, ConfigBase const& printObject );
