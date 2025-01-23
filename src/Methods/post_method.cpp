#include "../../include/methods.hpp"
#include <unistd.h>
#include <fcntl.h>

static std::string get_extension( std::string const &filename ) {

	// Find last occurrence of "."
    std::string::size_type pos = filename.rfind('.');
    
    // Check if "." exists and its not the first position
    if ( pos != std::string::npos && pos != 0 )
        return filename.substr(pos + 1);
    
    // If no valid extension found return empty string
    return "";

}

static void	post_create( std::string path, Client& client, HTTPRequest* request, const Location* location )
{
	/* NOTE: 2.1. Case is not CGI, so its a file */
	struct stat file_data;
	std::pair<bool, bool> file_status = check_mode( path, &file_data, S_IWUSR );

	/* Path does not exist */
	if ( !file_status.first )
		return error_send( client, 404, location->get_error_page( 404 ), request );

	/* Path does not have enough permissions (write and execute for upload files) */
	if ( !file_status.second )
		return error_send( client, 403, location->get_error_page( 403 ), request );

	/* If path end is a file get that filename, otherwise get name from get_filename function */
	std::string dest_filename;
	if ( !S_ISDIR(file_data.st_mode) )
		dest_filename = path;
	else {
		if ( !(file_data.st_mode & S_IXUSR) )
			return error_send( client, 403, location->get_error_page( 403 ), request );

		std::string new_path = ( path[ path.size() - 1 ] == '/' ) ? path : path + "/";

		if ( request->get_filename().find_last_of('/') == std::string::npos )
			dest_filename = new_path + request->get_filename();
		else
			dest_filename = new_path + request->get_filename().substr( request->get_filename().find_last_of('/') + 1 );
	}
	
	/* Change file path */
	request->move_body_file( dest_filename );

	/* Return response OK */
	error_send( client, 201, location->get_error_page( 201 ), request );
}

static CGIClient*	post_cgi( std::string path, std::string cgi_path, Client& client, HTTPRequest* request, const Location* location )
{
	/* NOTE: Open the pipes */
	int pipes[2];
	pipe(pipes);
	int flags = fcntl(pipes[0], F_GETFL, 0);
	fcntl(pipes[0], F_SETFL, flags | O_NONBLOCK);

	/* NOTE: Divide the execution on parent and child */
	pid_t pid = fork();
	if ( pid == 0 ) /* Child */
	{
		close(pipes[0]);

		char *args[3] = {
			const_cast<char*>(cgi_path.c_str()),
			const_cast<char*>(path.c_str()),
			NULL
		};

		/* Redirecciones */
		int infile = open(request->get_filename().c_str(), O_RDONLY);
		dup2(infile, STDIN_FILENO);
		close(infile);
		dup2(pipes[1], STDOUT_FILENO);

		int	dev_null = open("/dev/null", O_WRONLY);
		if (dev_null > 0)
		{
			dup2(dev_null, STDERR_FILENO);
			close(dev_null);
		}

		/* Ejecucion */
		execve(cgi_path.c_str(), args, environ);
		exit(1);
	}
	
	/* Parent */
	close(pipes[1]);

	return new CGIClient( client, const_cast<Location*>(location), pid, pipes[0], request );
}

CGIClient*	post_method( std::string path, Client& client, HTTPRequest* request)
{
	/* NOTE: 1. Get extension of the route */
	// std::string extension = get_extension( request->get_filename() );
	std::string extension = get_extension( path );

	/* NOTE: 2. Get location from request and consult CGI's map */
	std::pair<bool, const Location *> location = client.get_server().get_location( request->get_path() );
	std::string cgi_found = location.second->get_cgi( extension );

	CGIClient *cgiclient = NULL;
	if ( cgi_found.empty() )
		post_create( path, client, request, location.second );
	else
		cgiclient = post_cgi( path, cgi_found, client, request, location.second );

	return cgiclient;
}
