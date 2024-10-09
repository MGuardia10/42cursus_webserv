#include "../../include/ConfigBase.hpp"
#include <algorithm>
#include <sstream>

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

std::string	ConfigBase::print(void) const
{
	std::stringstream	ss;
	std::string	buffer = "";

	/* Root */
	buffer = "\t- Root: " + _root + "\n";

	/* Client max body */
	ss << _client_max_body_size;
	buffer += "\t- Client max body: " + ss.str() + "\n";
	
	/* Autoindex */
	buffer += "\t- Autoindex: " + (_autoindex == true ? std::string("true") : std::string("false"));

	/* Index */
	buffer += "\t- Index:\n";
	for (std::vector<std::string>::const_iterator it; it != _indexes.end(); it++)
		buffer += "\t\t· \"" + *it + "\"\n";

	/* Error_pages */
	buffer += "\t- Error pages:\n";
	for (std::map<int, std::string>::const_iterator it; it != _error_pages.end(); it++)
	{
		ss.clear();
		ss << it->first;
		buffer += "\t\t· " + ss.str() + ": \"" + it->second + "\"";
	}

	/* Return data */
	ss.clear();
	ss << _return_data.code;
	buffer += "\t- Return data:\n\t\t· " + ss.str() + "\n\t\t· Text: " + _return_data.text + "\n";

	/* CGIs*/
	buffer += "\t- CGI:\n";
	for (std::map<std::string, std::string>::const_iterator it; it != _cgi.end(); it++)
		buffer += "\t\t· " + it->first + ": \"" + it->second + "\"";

	return buffer;
}

std::ostream&	operator<<( std::ostream& os, ConfigBase const& printObject )
{
	os << printObject.print();
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
ConfigBase::ReturnData const&	ConfigBase::get_return( void ) const { return _return_data; }
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
