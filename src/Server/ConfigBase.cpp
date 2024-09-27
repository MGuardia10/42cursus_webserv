#include "../../include/ConfigBase.hpp"

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

ConfigBase::ConfigBase( void ) :
	_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT),
	_root(ROOT_DEFAULT),
	_autoindex(AUTOINDEX_DEFAULT)
{
	_indexes.clear();
	_cgi.clear();
}

ConfigBase::ConfigBase( ConfigBase const& other )
{
	/* TODO */
	(void) other;
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

/**********************************/
/* NOTE: Copy assignment operator */
/**********************************/
ConfigBase&	ConfigBase::operator=( ConfigBase const& other )
{
	if (this != &other)
	{
		/* TODO */
	}
	return (*this);
}

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
