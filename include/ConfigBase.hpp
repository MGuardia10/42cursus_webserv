#pragma once

#include <iostream>
#include <map>
#include <vector>
#include "default.hpp"

class ConfigBase
{
	public:
		struct ReturnData {
			int			code;
			std::string	text;
		};

	protected:
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
		std::map<std::string, std::string>	_cgi;

		/* Return field */
		ReturnData	_return_data;

		/* NOTE: Constructors and destructor */
		ConfigBase( void );
		virtual ~ConfigBase( void );

	public:

		/* NOTE: Member operators overloading */

		/* NOTE: Getters and setters */
		/* Error pages */
		std::map<int, std::string>	get_error_pages( void ) const;
		std::string					get_error_page( int code ) const;
		void						add_error_page( int code, std::string path );

		/* Client max size */
		size_t	get_client_max_size( void ) const;
		void	set_client_max_size( size_t client_max_size );

		/* Root */
		std::string	get_root( void ) const;
		void		set_root( std::string root );

		/* Index */
		std::vector<std::string>	get_indexes( void ) const ;
		void						add_index( std::string index );

		/* Autoindex */
		bool	get_autoindex( void ) const;
		void	set_autoindex( bool autoindex );

		/* CGI */
		std::map<std::string, std::string>	get_cgis( void ) const;
		std::string							get_cgi( std::string extension ) const;
		void								add_cgi( std::string extension, std::string path );

		/* Return */
		ReturnData const&	get_return( void ) const;
		void	set_return( ReturnData data );

		/* NOTE: Objects features */
		std::string	print( void ) const;
		std::string	print( std::string preline ) const;

		void	inherit( ConfigBase const& src );

		/* NOTE: Exceptions */

};

/* NOTE: Non-member operators overloading */
std::ostream&	operator<<( std::ostream& os, ConfigBase const& printObject );
