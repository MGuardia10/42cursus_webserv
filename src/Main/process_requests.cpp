#include "../../include/process_requests.hpp"
#include "../../include/validate_requests.hpp"
#include "../../include/Client.hpp"
#include "../../include/signals.hpp"
#include "../../include/colors.hpp"
#include "../../include/HTTPRequest.hpp"
#include "../../include/HTTPResponse.hpp"
#include "../../include/methods.hpp"
#include "../../include/default.hpp"
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

static void	remove_cgiclients( int fd, std::vector<pollfd>& fds, std::map<int, CGIClient*>& cgis)
{
	/* Remove for the pollfd vector */
	for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++ )
	{
		if ( (*it).fd == fd )
		{
			fds.erase(it);
			break ;
		}
	}

	/* Remove from the map */
	std::map<int, CGIClient*>::iterator current = cgis.find( fd );
	if (current == cgis.end())
		return ;
	CGIClient*	cgi = current->second;
	close( cgi->get_pipe_fd() );
	delete cgi;
	cgis.erase( current );
}

/**
 * @brief Funtion to save the new servers connections
 * 
 * @param[in] fd	File descriptor of the server affected
 * @param[in] servers	Map with the possible servers
 * @param[out] clients	Map with the clients
 * @param[out] pollfds	Vector with the pollfds
 * 
 * @return true if a connection has been added, false otherwise
*/
static bool	handle_new_connection( int fd, std::map<int, Server>& servers, std::map<int, Client>& clients, std::vector<pollfd>& pollfds )
{
	std::map<int, Server>::iterator server_it = servers.find(fd);
	if (server_it == servers.end())
		return false;

	/* New connection */
	int new_connection = accept(fd, NULL, NULL);
	if (DEBUG)
		std::cout << GREEN << "[ NEW CONNECTION ]" << RESET << " New client: " << new_connection << std::endl;

	/* Make the connection non blocking */
	int flags = fcntl(new_connection, F_GETFL, 0);
	fcntl(new_connection, F_SETFL, flags | O_NONBLOCK);

	/* Creation of the client to save the info */
	Client new_client = Client(new_connection, server_it->second);
	if (DEBUG)
		std::cout << new_client << std::endl;
	clients.insert(std::pair<int, Client>(new_connection, new_client));

	/* Add the fd to the pollfd list */
	pollfd client_pollfd = {
		.fd = new_connection,
		.events = POLLIN,
		.revents = 0
	};
	pollfds.push_back(client_pollfd);

	return true;
}

/**
 * @brief Funtion to handle the clients requests
 * 
 * @param[in] fd			File descriptor of the client affected
 * @param[in,out] clients	Map with the clients
 * 
 * @return A pair of items: the first one is a bool that indicates if the connection
 * 			has to be closed; the second one is a pointer to a CGIClient, if one is created
*/
static std::pair<bool, CGIClient*>	handle_clients_request( int fd, std::map<int, Client>& clients )
{
	std::string response;
	std::map<int, Client>::iterator client_it = clients.find(fd);

	if (client_it == clients.end())
		return std::pair<bool, CGIClient*>(false, NULL);
	if (DEBUG)
		std::cout << YELLOW << "[ NEW REQUEST ]" << RESET << " New request by " << fd << std::endl;

	/* NOTE: Save all the request */
	HTTPRequest*	request = client_it->second.get_request();
	if (!request)
	{
		request = new HTTPRequest();
		client_it->second.set_request(request);
	}

	/* If the request is not finished or is a close connection, do some things */
	request->process_request( fd );
	if (!request->check_finished())
		return std::pair<bool, CGIClient*>(false, NULL);

	if (request->check_closed())
	{
		delete request;
		if (DEBUG)
			std::cout <<std::endl << RED"[ CLOSED CONNECTION ]" << RESET << " The connection with " << fd << " has been closed" << std::endl;
		clients.erase(client_it);
		close(fd);
		return std::pair<bool, CGIClient*>(true, NULL);
	}

	/* NOTE: Validate the request */
	RequestData request_data = validate_request( client_it->second, request );

	/* Return case */
	if ( request_data.returnData.code != -1 ) {

		/* Get response */
		response = HTTPResponse::get_return_response( &request_data.returnData, client_it->second.get_cookie() );

		send(fd, response.c_str(), response.size(), MSG_NOSIGNAL);
		
		/* Return false */
		delete request;
		client_it->second.set_request(NULL);
		return std::pair<bool, CGIClient*>(false, NULL);;
	}

	/* Error_page case */
	if ( request_data.errorData.code != -1 ) {

		/* Set offset */
		long long offset = 0;

		do
		{
			/* Get error page */
			std::pair<long long, std::string> data;
			data = HTTPResponse::get_error_page_response( request_data.errorData.code, request_data.errorData.path, client_it->second.get_cookie(), offset, request_data.methods );
			
			/* Update offset and response values */
			offset = data.first;
			response = data.second;

			/* Send response */
			if (send(fd, response.c_str(), response.size(), MSG_NOSIGNAL) == -1)
				break ;

		} while (offset != 0);

		delete request;
		client_it->second.set_request(NULL);
		return std::pair<bool, CGIClient*>(false, NULL);
	}
	
	/* NOTE: Get the full path */
	std::pair<bool, Location const*> location = client_it->second.get_server().get_location( request->get_path() );
	std::string	full_path;

	std::string root = location.second->get_root();
	if (root == "")	root = ".";
	std::string route = location.second->get_route();
	std::string alias = location.second->get_alias();

	/* Make the path taking into account the alias of the location */
	if (alias == "") {
		if ( route == request->get_path() )
			full_path = root + route;
		else
			full_path = root + route + request->get_path().replace( 0 , route.size(), ""  );
	}
	else {
		/* Get path without route to insert alias */
		std::string path = request->get_path().replace( 0 , route.size(), ""  );
	
		if ( alias[ alias.size() - 1 ] == '/' ) {
			if ( !path.empty() && path[0] == '/' ) /* Case alias has end "/" and path has end "/" */
				full_path = root + alias + path.substr( 1, path.size() - 1 );
			else	/* Case alias has end "/" and path has no start "/" */
				full_path = root + alias + path;
		} else if ( !path.empty() && path[0] == '/' ) { /* Case alias has no end "/" and path starts with "/" */
			full_path = root + alias + path;
		} else	/* Case alias has no end "/" and path starts with no "/"  */
			full_path = root + alias + "/" + path;

		/* remove last "/" */
		full_path = (full_path.at( full_path.size() - 1 ) == '/' ) ? full_path.substr( 0, full_path.size() - 1 ) : full_path;
	}

	/* NOTE: Check the method and call a function */
	CGIClient	*cgiclient = NULL;
	if (request->get_method() == "GET")
		get_method( full_path, client_it->second, request );
	else if (request->get_method() == "POST")
		cgiclient = post_method( full_path, client_it->second, request );
	else /* DELETE */
		delete_method( full_path, client_it->second, request );

	/* Delete the request data */
	client_it->second.set_request(NULL);
	if (!cgiclient)
		delete request;
	return std::pair<bool, CGIClient*>(false, cgiclient);
}

/**
 * @brief	Function to manage the CGI actions
 * 
 * @param	fd File descriptor to check
 * @param	cgis Map with all the cgis data
 * @param	read_data Boolean to take into a count the read of the data or not
 * 
 * @return	Boolean that indicates if the element has to be removed or not
 */
static bool	handle_cgi( int fd, std::map<int, CGIClient*>& cgis, bool read_data )
{
	/* NOTE: Check if the fd is from a CGI */
	std::map<int, CGIClient*>::iterator cgi_it = cgis.find( fd );
	if ( cgi_it == cgis.end() )
		return false;
	CGIClient* current_cgi = cgi_it->second;

	/* NOTE: Check the timeout */
	if (current_cgi->has_timeout())
	{
		/* Close the process */
		kill(current_cgi->get_pid(), SIGTERM);
		if (waitpid(current_cgi->get_pid(), NULL, WNOHANG) == 0)
		{
			kill(current_cgi->get_pid(), SIGKILL);
			waitpid(current_cgi->get_pid(), NULL, WNOHANG);
		}

		/* Return a 504 */
		error_send( current_cgi->get_client(), 504, current_cgi->get_location()->get_error_page(504), current_cgi->get_request() );
		return true;
	}

	if (!read_data)
		return false;

	/* Read the buffer data */
	char buffer[1025];
	int bytes_recieved;
	do
	{
		bytes_recieved = read( current_cgi->get_pipe_fd(), buffer, 1024 );
		if (bytes_recieved > 0)
		{
			buffer[bytes_recieved] = '\0';
			current_cgi->add_data(buffer);
		}
	} while (bytes_recieved > 0);
	
	/* Check if the process finished */
	int status;
	int result = waitpid( current_cgi->get_pid(), &status, WNOHANG );
	if (result <= 0)
		return false;
	
	/* NOTE: Prepare the response, taking into account the status (child exit) value */
	if (DEBUG)
		std::cout << GRAY << "[ CGI ]" << RESET << " Finished with status " << result << std::endl;
	if (WEXITSTATUS(status) == 0)
	{
		std::string response = HTTPResponse::get_cgi_data_response( current_cgi->get_data(), current_cgi->get_client().get_cookie());
		send( current_cgi->get_client().get_fd(), response.c_str(),response.size(), MSG_NOSIGNAL);
	}
	else
	{
		/* Error */
		error_send(current_cgi->get_client(), 500, current_cgi->get_location()->get_error_page(500), current_cgi->get_request());
	}

	return true;
}

/**
 * @brief Function that handles the poll timeout
 * 
 * @param	fds Vector with all the fds
 * @param	cgis Map wit the client CGI objects
 */
static void	handle_poll_timeout( std::vector<pollfd>& fds, std::map<int, CGIClient*>& cgis )
{
	/* NOTE: Check the cgis */
	bool	changes;
	do
	{
		changes = false;
		for(std::map<int, CGIClient*>::iterator it = cgis.begin(); it != cgis.end(); it++)
		{
			if (handle_cgi( it->first, cgis, false ))
			{
				changes = true;
				remove_cgiclients( it->first, fds, cgis );
				break ;
			}
		}
	} while(changes);

}

void	process_requests(std::vector<Server> servers_vector)
{
	/* Storage variables */
	std::map<int, Server> servers;
	std::map<int, Client> clients;
	std::map<int, CGIClient*> cgis;
    std::vector<pollfd> pollfds;

	/* NOTE: Init the maps */
	clients.clear();
	cgis.clear();
	servers.clear();
	for (std::vector<Server>::iterator servers_it = servers_vector.begin(); servers_it != servers_vector.end(); servers_it++)
	{
		std::vector<int> sockets = (*servers_it).get_sockets();

		for (std::vector<int>::iterator sockets_it = sockets.begin(); sockets_it != sockets.end(); sockets_it++)
			servers.insert(std::pair<int, Server>(*sockets_it, *servers_it));
	}

	/* NOTE: Insert all the fds on the pollfd vector */
	pollfds.clear();
	for (std::map<int, Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		pollfd current_fd;
		current_fd.fd = it->first;
		current_fd.events = POLLIN;
		pollfds.push_back(current_fd);
	}

	/* NOTE: Request handler loop */
	int res;
	while (!sigint_signal)
	{
		if (DEBUG)
			std::cout << CYAN << "\n[ POLL ]" << RESET << " Waiting for changes... (" << pollfds.size() << ")\n\t- Servers: " << servers.size() << "\n\t- Clients: " << clients.size() << "\n\t- CGIs: " << cgis.size() << std::endl;
		res = poll(&pollfds[0], pollfds.size(), POLL_TIMEOUT);
		if (res < 0)
		{
			if (!sigint_signal)
				std::cout << "Error on the poll function" << std::endl;
			break ;
		}
		
		if (res == 0)
		{
			handle_poll_timeout( pollfds, cgis );
			continue ;
		}

		/* Check what fd has changed */
		for (std::vector<pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); it++)
		{
			/* Check if the current fd hsa been modified */
			if (it->revents & POLLIN || it->revents & POLLHUP)
			{
				/* Handle CGI */
				if (handle_cgi( (*it).fd, cgis, true ))
				{
					remove_cgiclients( (*it).fd, pollfds, cgis );
					break ;
				}

				/* Handle the client requests */
				std::pair<bool, CGIClient*> connection_data = handle_clients_request( (*it).fd, clients );
				if (connection_data.first)
				{
					pollfds.erase(it);
					break ;
				}

				if (connection_data.second)
				{
					cgis.insert(std::pair<int, CGIClient*>( connection_data.second->get_pipe_fd(), connection_data.second ));
					pollfd current_pollfd;
					current_pollfd.fd = connection_data.second->get_pipe_fd();
					current_pollfd.events = POLLIN | POLLHUP;
					pollfds.push_back(current_pollfd);
					break ;
				}

				/* Handle the new connections */
				if (handle_new_connection((*it).fd, servers, clients, pollfds))
					break ;
			}
		}
	}

	/* Close the actives cgis */
	bool changes;
	do
	{
		changes = false;
		for (std::map<int, CGIClient*>::iterator it = cgis.begin(); it != cgis.end(); it++)
		{
			CGIClient* current_cgi = it->second;

			/* Close the process */
			kill(current_cgi->get_pid(), SIGTERM);
			if (waitpid(current_cgi->get_pid(), NULL, WNOHANG) == 0)
			{
				kill(current_cgi->get_pid(), SIGKILL);
				waitpid(current_cgi->get_pid(), NULL, WNOHANG);
			}

			/* Return a 504 */
			error_send( current_cgi->get_client(), 504, current_cgi->get_location()->get_error_page(504), current_cgi->get_request() );
			remove_cgiclients( it->first, pollfds, cgis );
			break ;
		}
	} while (changes);

	/* NOTE: Close the client fds, sending a close response */
	std::string response;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		response = HTTPResponse::get_close_connection_template(it->second.get_cookie());
		send(it->first, response.c_str(), response.size(), MSG_NOSIGNAL);
		close(it->first);
	}
	
}
