#include "../../include/ConfigBase.hpp"
#include <algorithm>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

ConfigBase::ConfigBase( void ) :
	_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT),
	_root(ROOT_DEFAULT),
	_autoindex(AUTOINDEX_DEFAULT)
{
	/* Empty the vectors */
	_indexes.clear();
	_cgi.clear();

	/* Init the return struct */
	_return_data.code = -1;
	_return_data.text = "";
}

ConfigBase::~ConfigBase( void )
{
	/* TODO */
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/***********************/
/* NOTE: '<<' operator */
/***********************/
std::ostream&	operator<<( std::ostream& os, ConfigBase const& printObject )
{
	/* TODO */
	(void) printObject;
	return (os);
};

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

/*********************/
/* NOTE: Error pages */
/*********************/
std::map<int, std::string>	ConfigBase::get_error_pages( void ) const	{ return _error_pages; }
std::string					ConfigBase::get_error_page( int code ) const
{
	std::map<int, std::string>::const_iterator it = _error_pages.find( code );
	return (it == _error_pages.end() ? "" : it->second);
}
void						ConfigBase::add_error_page( int code, std::string path )
{
	if (_error_pages.find(code) == _error_pages.end())
		_error_pages.insert(std::pair<int, std::string>(code, path));
}

/*************************/
/* NOTE: Client Max Size */
/*************************/
size_t	ConfigBase::get_client_max_size( void ) const { return _client_max_body_size; }
void	ConfigBase::set_client_max_size( size_t client_max_size ) { _client_max_body_size = client_max_size; }

/**************/
/* NOTE: Root */
/**************/
std::string	ConfigBase::get_root( void ) const	{ return _root; }
void		ConfigBase::set_root( std::string root ) { _root = root; }

/*****************/
/* NOTE: Indexes */
/****************/
std::vector<std::string>	ConfigBase::get_indexes( void ) const { return _indexes; }
void	ConfigBase::add_index( std::string index )
{
	if (std::find(_indexes.begin(), _indexes.end(), index) == _indexes.end())
		_indexes.push_back(index);
}

/*******************/
/* NOTE: Autoindex */
/*******************/
bool	ConfigBase::get_autoindex( void ) const		{ return _autoindex; }
void	ConfigBase::set_autoindex( bool autoindex )	{ _autoindex = autoindex; }

/*************/
/* NOTE: CGI */
/*************/
std::map<std::string, std::string>	ConfigBase::get_cgis( void) const	{ return _cgi; }
std::string							ConfigBase::get_cgi( std::string extension ) const
{
	std::map<std::string, std::string>::const_iterator it = _cgi.find(extension);
	return (it == _cgi.end() ? "" : it->second);
}
void								ConfigBase::add_cgi( std::string extension, std::string path )
{
	if (_cgi.find(extension) == _cgi.end())
		_cgi.insert(std::pair<std::string, std::string>(extension, path));
}

/****************/
/* NOTE: Return */
/****************/
ConfigBase::ReturnData const*	ConfigBase::get_return( void ) const { return &(_return_data); }
void							ConfigBase::set_return( ReturnData data )
{
	_return_data.code = data.code;
	_return_data.text = data.text;
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
