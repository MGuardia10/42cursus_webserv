#pragma once

/* TODO: Defaults error_page's */
# define ERROR_PAGE_DEFAULT	NULL

/* Default max size of a HTTP request -> 100 bytes/characters */
/* TODO: check the nginx value */
# define CLIENT_MAX_BODY_SIZE_DEFAULT	100

/* TODO: Default return */
# define RETURN_DEFAULT	NULL

/* Default alias -> None */
# define ALIAS_DEFAULT					""

/* Default folder where the pages will be searched */
# define ROOT_DEFAULT					"pages/"

/* Default autoindex (the folders won't saw their files) */
# define AUTOINDEX_DEFAULT				false
