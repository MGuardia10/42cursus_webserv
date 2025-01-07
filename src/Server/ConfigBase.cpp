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
	_error_pages.clear();

	/* Init the return struct */
	_return_data.code = -1;
	_return_data.text = "";

	/* Append the GET method */
	_methods.push_back("GET");

	/* Init the array to know what data is iniciated */
	for (int i = 0; i < TOTAL_INDEX; i++)
		_inicializated[i] = false;
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

std::string	ConfigBase::print( void ) const { return this->print(""); }
std::string	ConfigBase::print( std::string preline ) const
{
	std::stringstream	ss;
	std::string	buffer = "";

	/* Root */
	buffer = "\t" + preline + "- Root: \"" + _root + "\"\n";

	/* Client max body */
	ss << _client_max_body_size;
	buffer += "\t" + preline + "- Client max body: " + ss.str() + "\n";
	
	/* Autoindex */
	buffer += "\t" + preline + "- Autoindex: " + (_autoindex == true ? std::string("true") : std::string("false")) + "\n";

	/* Index */
	buffer += "\t" + preline + "- Index:";
	if (_indexes.size() == 0)
		buffer += " None";
	buffer += "\n";
	for (std::vector<std::string>::const_iterator it = _indexes.begin(); it != _indexes.end(); it++)
		buffer += "\t\t" + preline + "· \"" + *it + "\"\n";

	/* Methods */
	buffer += "\t" + preline + "- Methods:";
	if (_methods.size() == 0)
		buffer += "None";
	buffer += "\n";
	for (std::vector<std::string>::const_iterator it = _methods.begin(); it != _methods.end(); it++)
		buffer += "\t\t" + preline + "· " + *it + "\n";

	/* Error_pages */
	buffer += "\t" + preline + "- Error pages:";
	if (_error_pages.size() == 0)
		buffer += " None";
	buffer += "\n";
	for (std::map<int, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
	{
		ss.str("");
		ss << it->first;
		buffer += "\t\t" + preline + "· " + ss.str() + ": \"" + it->second + "\"\n";
	}

	/* Return data */
	ss.str("");
	ss << _return_data.code;
	buffer += "\t" + preline + "- Return data:\n\t\t" +
		preline + "· Code: " + ss.str() + "\n\t\t" +
		preline + "· Text: \"" + _return_data.text + "\"\n";

	/* CGIs*/
	buffer += "\t" + preline + "- CGI:";
	if (_cgi.size() == 0)
		buffer += " None";
	buffer += "\n";
	for (std::map<std::string, std::string>::const_iterator it = _cgi.begin(); it != _cgi.end(); it++)
		buffer += "\t\t" + preline + "· " + it->first + ": \"" + it->second + "\"\n";

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
	std::map<int, std::string>::iterator it = _error_pages.find(code);

	if (it == _error_pages.end())
		_error_pages.insert(std::pair<int, std::string>(code, path));
	else
		it->second = path;
}

/*************************/
/* NOTE: Client Max Size */
/*************************/
size_t	ConfigBase::get_client_max_size( void ) const { return _client_max_body_size; }
void	ConfigBase::set_client_max_size( size_t client_max_size )
{
	_client_max_body_size = client_max_size;
	_inicializated[CLIENT_MAX_BODY_SIZE_INDEX] = true;
}

/**************/
/* NOTE: Root */
/**************/
std::string	ConfigBase::get_root( void ) const	{ return _root; }
void		ConfigBase::set_root( std::string root ) {
	_root = root;
	_inicializated[ROOT_INDEX] = true;
}

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
void	ConfigBase::set_autoindex( bool autoindex )	{
	_autoindex = autoindex;
	_inicializated[AUTOINDEX_INDEX] = true;
}

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
	std::map<std::string, std::string>::iterator it = _cgi.find(extension);

	if (it == _cgi.end())
		_cgi.insert(std::pair<std::string, std::string>(extension, path));
	else
		it->second = path;
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

/*****************/
/* NOTE: Methods */
/*****************/
std::vector<std::string>	ConfigBase::get_methods( void ) const { return _methods; }
bool						ConfigBase::has_method( std::string method ) const
{
	return std::find(_methods.begin(), _methods.end(), method) != _methods.end();
}

void						ConfigBase::add_method( std::string method )
{
	if (!_inicializated[METHODS_INDEX])
	{
		_methods.clear();
		_inicializated[METHODS_INDEX] = true;
	}

	/* Check if the method is already on the vector */
	if (has_method(method))
		return;

	/* Append */
	_methods.push_back(method);
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                      Object features                              */
/*============================================================================*/

void	ConfigBase::inherit( ConfigBase const& src )
{
	/* Copy the client max body size */
	if (!_inicializated[CLIENT_MAX_BODY_SIZE_INDEX])
		_client_max_body_size = src._client_max_body_size;

	/* Copy the root */
	if (!_inicializated[ROOT_INDEX])
		_root = src._root;

	/* Copy the autoindex */
	if (!_inicializated[AUTOINDEX_INDEX])
		_autoindex = src._autoindex;

	/* Copy the error pages */
	for (
		std::map<int, std::string>::const_iterator it = src._error_pages.begin();
		it != src._error_pages.end();
		it++
	)
	{
		if (_error_pages.find(it->first) == _error_pages.end())
			_error_pages.insert(std::pair<int, std::string>(it->first, src._root + it->second));
	}

	/* Copy the indexes */
	for (
		std::vector<std::string>::const_iterator it = src._indexes.begin();
		it != src._indexes.end();
		it++
	)
	{
		if (std::find(_indexes.begin(), _indexes.end(), *it) == _indexes.end())
			_indexes.push_back(src._root + "/" + *it);
	}

	/* Copy the cgis */
	for (
		std::map<std::string, std::string>::const_iterator it = src._cgi.begin();
		it != src._cgi.end();
		it++
	)
	{
		if (_cgi.find(it->first) == _cgi.end())
			_cgi.insert(std::pair<std::string, std::string>(it->first, it->second));
	}

	/* Copy the return data */
	if (_return_data.code == -1 && _return_data.text == "")
	{
		_return_data.code = src._return_data.code;
		_return_data.text = src._return_data.text;
	}

	/* Copy the methods */
	if (!_inicializated[METHODS_INDEX])
	{
		_methods.clear();

		for (std::vector<std::string>::const_iterator it = src._methods.begin(); it != src._methods.end(); it++)
		{
			if (!has_method(*it))
				_methods.push_back(*it);
		}
	}

}

/*==========*/
/* !SECTION */
/*==========*/
/*============================================================================*/
/* SECTION:                        Exceptions                                 */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*==========*/
