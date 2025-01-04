#include "../../include/HTTPResponse.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#include <sys/types.h>
#include <dirent.h>

/*============================================================================*/
/* SECTION:               Constructors and destructor                         */
/*============================================================================*/

std::map<int, std::string>	HTTPResponse::_codes;
std::map<std::string, std::string>	HTTPResponse::_extensions;

HTTPResponse::HTTPResponse( void )
{}

HTTPResponse::~HTTPResponse( void )
{}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                   Operator overloading                            */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                    Getters and setters                            */
/*============================================================================*/

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                           Loads                                   */
/*============================================================================*/

/* NOTE: Loads */

/**
 * @brief Generic function to read de csv config files
 * 
 * @param	file Reference to the config file
 * @param	delimiter	Delimited of the rows
 * 
 * @return A pair with false and an empty map if there is any problem during the reading;
 * 			Otherwise, true and the map with the data
 */
static std::pair< bool, std::map<std::string, std::string> > read_csv_files( std::ifstream& file, std::string delimiter )
{
	std::map<std::string, std::string> data;
	std::string	buffer;
	std::string first, second;
	size_t	index;

	getline(file, buffer);
	while (getline(file, buffer))
	{
		if (buffer.empty() || buffer[0] == '#')
			continue ;
		
		/* Search the delimiter */
		index = buffer.find(delimiter);
		if (index == std::string::npos)
			return std::pair< bool, std::map<std::string, std::string> >(false, std::map<std::string, std::string>());
		
		/* Get the two parts */
		first = buffer.substr(0, index);
		second = buffer.substr(index + 1, buffer.size());
		if (first.empty() || second.empty())
			return std::pair< bool, std::map<std::string, std::string> >(false, std::map<std::string, std::string>());

		/* Insert the parts */
		data.insert(std::pair<std::string, std::string>(first, second));
	}
	return std::pair< bool, std::map<std::string, std::string> >(true, data);
}

/** Function to load all the data necessary to prepare the responses */
bool	HTTPResponse::load_data( void )
{
	return load_codes() && load_extensions();
}

/** Function to load the errors types. It reads the info on the ERROR_PATH file. */
bool	HTTPResponse::load_codes( void )
{
	std::ifstream	file;

	file.open( ERRORS_PATH );
	if (!file.is_open())
		return false;
	
	/* Get the data */
	std::pair< bool, std::map<std::string, std::string> > res = read_csv_files(file, ",");
	file.close();
	if (!res.first)
		return false;
	
	/* Save the data */
	_codes.clear();
	for (std::map<std::string, std::string>::iterator it = res.second.begin(); it != res.second.end(); it++)
		_codes.insert(std::pair<int, std::string>(atoi(it->first.c_str()), it->second));

	return true;
}

/** Function to load the errors types. It reads the info on the EXTENSIONS_PATH file. */
bool	HTTPResponse::load_extensions( void )
{
	std::ifstream	file;

	file.open( EXTENSIONS_PATH );
	if (!file.is_open())
		return false;
	
	/* Get the data */
	std::pair< bool, std::map<std::string, std::string> > res = read_csv_files(file, ",");
	file.close();
	if (!res.first)
		return false;
	
	/* Save the data */
	_extensions = res.second;

	return true;
}

/*==========*/
/* !SECTION */
/*============================================================================*/
/* SECTION:                           Pages                                   */
/*============================================================================*/

/**
 * @brief Function to save the headers on a map as a string
 * 
 * @param	headers The headers on a map
 * 
 * @return The complete headers as string
 */
std::string	headers_map_to_string( std::map<std::string, std::string> headers )
{
	std::string header = "";

	std::map<std::string, std::string>::iterator it = headers.find("Status");
	if (it != headers.end())
		header += it->second + "\r\n";
	
	for (it = headers.begin(); it != headers.end(); it++)
		if (it->first != "Status")
			header += it->first + ": " + it->second + "\r\n";
	
	return header + "\r\n";
}

/** Function to get all the basic/general headers. */
std::map<std::string, std::string>	HTTPResponse::get_default_headers( int code, std::string cookie, bool connection_alive )
{
	std::map<std::string, std::string> headers;

	/* Status line */
	std::stringstream ss;
	std::map<int, std::string>::iterator code_search = _codes.find( code );

	ss << code;
	headers["Status"] = "HTTP/1.1 " + ss.str() + " " +
		(code_search != _codes.end() ? code_search->second : std::string("Undefined"));

	/* Server name */
	headers["Server"] = "Webserv/1.0";

	/* Date */
	time_t now_long = time(NULL);
	tm *now_struct = gmtime(&now_long);
	char now_string[80] = {0};
	strftime(now_string, 80, "%a, %d %b %Y %H:%M:%S GMT", now_struct);
	headers["Date"] = std::string(now_string);

	/* Connection */
	headers["Connection"] = (connection_alive ? std::string("keep-alive") : std::string("close"));

	/* Cookie */
	if (!cookie.empty())
		headers["Set-Cookie"] = cookie;

	/* Cache Control */
	headers["Cache-Control"] = "no-cache";

	return headers;
}

/** Function to generate a general response, with a specified code and msg */
std::string HTTPResponse::get_response_template( int code, std::string msg, std::string cookie )
{
	std::map<std::string, std::string> header;
	std::string	body;
	std::stringstream ss;

	std::map<int, std::string>::iterator code_it;
	std::map<std::string, std::string>::iterator extension_it;

	/* Get the default headers */
	header = get_default_headers( code, cookie, true );

	/* Set the body */
	if (msg == "")
	{
		code_it = _codes.find( code );
		ss.str("");
		ss << code;
		msg = ss.str() + " - " + (code_it == _codes.end() ? "Undefined" : code_it->second);
	}
	body = "<html><head><title>Webserv</title></head><body><p>" + msg + "</p></body></html>";

	/* Add headers */
	ss.str("");
	ss << body.size();
	header["Content-Length"] = ss.str();
	extension_it = _extensions.find("html");
	header["Content-Type"] = (extension_it == _extensions.end() ? "text/html" : extension_it->second);

	/* Put everything together */
	return headers_map_to_string(header) + body;
}

/** Function that generates a response that indicates that the connection is closed */
std::string	HTTPResponse::get_close_connection_template( std::string cookie )
{
	std::map<std::string, std::string> header;

	/* General headers */
	header = get_default_headers( 200, cookie, false );

	return headers_map_to_string(header);
}

/** Function to return the data of a file */
std::pair<long long, std::string>	HTTPResponse::get_file_response( int code, std::string path, std::string cookie, long long offset )
{
	std::map<std::string, std::string> header;
	std::string body, response;
	std::ifstream	file;

	/* Check if the file exists */
	file.open( path.c_str() );
	if (!file.is_open())
	{
		response = get_response_template( 404, "The resource has not been found", cookie);
		return std::pair<long long, std::string>(-1, response);
	}

	/* If the offset is 0, generate the header */
	header.clear();
	if (offset == 0)
	{
		/* Default headers */
		header = get_default_headers( (code < 0 ? 200 : code), cookie, true );

		/* File length */
		struct stat data;
		stat( path.c_str(), &data );

		std::stringstream	ss;
		ss << data.st_size;
		header["Content-Length"] = ss.str();

		/* File extension */
		std::map<std::string, std::string>::iterator it;
		size_t index = path.rfind(".");
		if (index != std::string::npos)
			 it = _extensions.find(path.substr(index + 1, path.size()));
		header["Content-Type"] = (index != std::string::npos && it != _extensions.end() ? it->second : "text/plain");
	}

	/* Read and write on the buffer */
	std::vector<char>	buffer(CHUNK_SIZE);
	file.seekg( offset );
	file.read(&buffer[0], CHUNK_SIZE);
	body = std::string(&buffer[0], buffer.size());

	size_t	new_offset = (file.gcount() == CHUNK_SIZE) ? offset + CHUNK_SIZE : 0;
	response = (header.empty()) ? body : headers_map_to_string( header ) + body;

	/* Return the response, plus headers if it is neccesary */
	return std::pair<long long, std::string>( new_offset, response );
}

/** Function to generate the response of a return statement */
std::string	HTTPResponse::get_return_response( ConfigBase::ReturnData* data, std::string cookie )
{
	std::map<std::string, std::string> header;
	std::string response;

	if (!data)
		return "";

	/* Check if it is a redirection or a message */
	if (data->text.find("http://") != 0 && data->text.find("https://") != 0)
		response = get_response_template( data->code, data->text, cookie );
	else
	{
		/* Redirection */
		header = get_default_headers( data->code, cookie, false );
		header["Location"] = data->text;
		response = headers_map_to_string( header );
	}
	return response;
}

/** Function to return an error page */
std::pair<long long, std::string>	HTTPResponse::get_error_page_response( int code, std::string path, std::string cookie, long long offset )
{
	std::pair<long long, std::string> file_response;
	std::string	response;
	long long new_offset;

	/* Read the file; if an error is detected, */
	if (path != "")
		file_response = get_file_response( code, path, cookie, offset );

	if (path == "" || file_response.first < 0)
	{
		new_offset = 0;
		response = get_response_template( code, "", cookie );
	}
	else
	{
		new_offset = file_response.first;
		response = file_response.second;
	}
	return std::pair<long long, std::string>( new_offset, response );
}

/** Function to generate the autoindex of a specific path */
std::string	HTTPResponse::get_autoindex_response( std::string path, std::string cookie )
{
	std::string	head, body = "";
	std::map<std::string, std::string> header;
	std::stringstream	ss;

	DIR*	folder;
	std::vector< std::pair< dirent*, std::pair<std::string, std::string> > >	contents; /* < Data, <Type, Size> > */

	/* Open the directory */
	if (path[path.size() - 1] != '/')
		path += "/";
	folder = opendir(path.c_str());
	if (!folder)
		return get_response_template( 500, "", cookie );
	
	/* Read all the files/folders, saving the pointers. ALso check the max number of characters, for style */
	std::map<dirent*, std::string> files, folders; /* Data, */
	dirent*	current_item;
	contents.clear();
	while ((current_item = readdir( folder )))
	{
		std::string	name = current_item->d_name;
		if (name == "." || name == "..")
			continue ;
		
		struct stat item_data;
		stat(std::string(path + name).c_str(), &item_data);

		ss.str("");
		ss << item_data.st_size;

		if (S_ISDIR(item_data.st_mode))
			folders.insert(std::pair<dirent*, std::string>(current_item, ss.str()));
		else
			files.insert(std::pair<dirent*, std::string>(current_item, ss.str()));
	}

	for (std::map<dirent*, std::string>::iterator it = folders.begin(); it != folders.end(); it++)
		contents.push_back(std::pair< dirent*, std::pair<std::string, std::string> >(it->first, std::pair<std::string, std::string>("Folder", it->second)));

	for (std::map<dirent*, std::string>::iterator it = files.begin(); it != files.end(); it++)
		contents.push_back(std::pair< dirent*, std::pair<std::string, std::string> >(it->first, std::pair<std::string, std::string>("Files", it->second)));

	/* Print a table with all the file data */
	head = "<head><title>" + path + " autoindex - Webserv</title><style>"
		"body {\nfont-family: Arial, sans-serif;\nmargin: 20px;\nbackground-color: #f9f9f9;\n}\n"
		"table {\nwidth: 100%;\nborder-collapse: collapse;\nmargin: 20px 0;\nbackground-color: #ffffff;\nbox-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);\n}\n"
		"thead {\nbackground-color: #007bff;\ncolor: white;\n}\n"
		"thead th {\ntext-align: center;\n}\n"
		"th, td {\npadding: 10px;\ntext-align: left;\nborder: 1px solid #ddd;\n}\n"
		"tbody tr:nth-child(even) {\nbackground-color: #f2f2f2;\n}\n"
		"tbody tr:hover {\nbackground-color: #e0e0e0;\n}\n"
		"thead tr:hover {\nbackground-color: #007bff;\n}\n"
		"a {\ncolor: #007bff;\ntext-decoration: none;\nfont-weight: bold;\ntransition: color 0.3s, text-decoration 0.3s;\n}\n"
		"a:hover {\ncolor: #0056b3;\ntext-decoration: underline;\n}\n"
	"</style></head>";
	body = "<body><h1>Index of " + path + "</h1><hr><table><thead><tr><th>Name</th><th>Type</th><th>Size (bytes)</th></tr></thead><tbody>";

	for (std::vector< std::pair< dirent*, std::pair<std::string, std::string> > >::iterator it = contents.begin(); it != contents.end(); it++)
	{
		std::string href = std::string(it->first->d_name) + (it->second.first == "Folder" ? "/" : "");
		body += "<tr><td><a href=\"" + href + "\">" + href + "</a></td><td>" + it->second.first + "</td><td>" + it->second.second + "</td></tr>";
	}

	body += "</tbody></body></html>";
	body = head + body;

	/* Complete the headers */
	ss.str("");
	header = get_default_headers( 200, cookie, true );
	header["Content-Type"] = "text/html";
	ss << body.size();
	header["Content-Length"] = ss.str();

	/* Mix all */
	closedir( folder );
	return headers_map_to_string( header ) + body;
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
