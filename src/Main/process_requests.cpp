#include "../../include/process_requests.hpp"
#include "../../include/validate_requests.hpp"
#include "../../include/Client.hpp"
#include "../../include/signals.hpp"
#include "../../include/colors.hpp"
#include "../../include/HTTPRequest.hpp"
#include "../../include/HTTPResponse.hpp"
#include "../../include/methods.hpp"
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>

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
bool	handle_new_connection( int fd, std::map<int, Server>& servers, std::map<int, Client>& clients, std::vector<pollfd>& pollfds )
{
	std::map<int, Server>::iterator server_it = servers.find(fd);
	if (server_it == servers.end())
		return false;

	/* New connection */
	int new_connection = accept(fd, NULL, NULL);
	std::cout << GREEN << "[ NEW CONNECTION ]" << RESET << " New client: " << new_connection << std::endl;

	/* Make the connection non blocking */
	int flags = fcntl(new_connection, F_GETFL, 0);
	fcntl(new_connection, F_SETFL, flags | O_NONBLOCK);

	/* Creation of the client to save the info */
	Client new_client = Client(new_connection, server_it->second);
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
 * @return true if a connection has been closed, false otherwise
*/
bool	handle_clients_request( int fd, std::map<int, Client>& clients )
{
	std::string response;
	std::map<int, Client>::iterator client_it = clients.find(fd);

	if (client_it == clients.end())
		return false;
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
		return false;

	if (request->check_closed())
	{
		delete request;
		std::cout <<std::endl << RED"[ CLOSED CONNECTION ]" << RESET << " The connection with " << fd << " has been closed" << std::endl;
		clients.erase(client_it);
		close(fd);
		return true;
	}

	/* NOTE: Validate the request */
	RequestData request_data = validate_request( client_it->second, request );

	/* Return case */
	if ( request_data.returnData.code != -1 ) {

		/* Get response */
		response = HTTPResponse::get_return_response( &request_data.returnData, client_it->second.get_cookie() );

		send(fd, response.c_str(), response.size(), 0);
		
		/* Return false */
		delete request;
		client_it->second.set_request(NULL);
		return false;
	}

	/* Error_page case */
	if ( request_data.errorData.code != -1 ) {

		/* Set offset */
		long long offset = 0;

		do
		{
			/* Get error page */
			std::pair<long long, std::string> data;
			data = HTTPResponse::get_error_page_response( request_data.errorData.code, request_data.errorData.path, client_it->second.get_cookie(), offset );
			
			/* Update offset and response values */
			offset = data.first;
			response = data.second;

			/* Send response */
			send(fd, response.c_str(), response.size(), 0);

		} while (offset != 0);

		delete request;
		client_it->second.set_request(NULL);
		return false;
	}
	
	/* NOTE: Got the full path */
	std::pair<bool, Location const*> location = client_it->second.get_server().get_location( request->get_path() );
	std::string	full_path;

	std::string alias = location.second->get_alias();
	std::string root = location.second->get_root();
	if (root == "")	root = ".";
	std::string route = location.second->get_route();
	if (route == "/") route = "";

	/* Mkae the path takeing into account the alias of the location */
	if (alias == "")
		full_path = root + route + request->get_path();
	else
	{
		/* Get path withoute route to insert alias */
		std::string path = request->get_path().replace( 0 , route.size(), ""  );
		
		/* Create full path */
		full_path = root + alias + path;
		
		/* remove last "/" */
		full_path = (full_path.at( full_path.size() - 1 ) == '/' ) ? full_path.substr( 0, full_path.size() - 1 ) : full_path;
	}

	std::cout << "Full path: " << full_path << std::endl;
	
	/* TODO: Check the method and call a function */
	if (request->get_method() == "GET")
		get_method( full_path, client_it->second, request );
	else if (request->get_method() == "POST")
		post_method( full_path, client_it->second, request );
	else /* DELETE */
		delete_method( full_path, client_it->second, request );
	
	// std::cout << "Preparing response" << std::endl;
	// std::string response;
	// response =
	// 	"HTTP/1.1 200 OK\r\n"
	// 	"Content-Type: text/html\r\n"
	// 	"Connection: keep-alive\r\n"
	// 	"Set-Cookie: " + client_it->second.get_cookie() + "\r\n"
	// 	"Content-Length: 210\r\n"
	// 	// "Content-Length: 179\r\n"
	// 	"\r\n"
	// 	"<form action=\"/\" method=\"POST\" enctype=\"multipart/form-data\"><input type=\"text\" name=\"username\" placeholder=\"Enter your name\"><input type=\"file\" name=\"uploaded_file\"><button type=\"submit\">Upload</button></form>";
		// "<form action=\"/\" method=\"POST\"><label for=\"mensaje\">Mensaje:</label><input type=\"text\" id=\"mensaje\" name=\"mensaje\" required><button type=\"submit\">Enviar</button></form>";
	// std::string response = HTTPResponse::get_response_template( 200, "Test de template", client_it->second.get_cookie());

		// /* Set offset */
		// long long offset = 0;

		// /* Loop until offset 0 again */
		// do
		// {
		// 	std::pair<long long, std::string> data = HTTPResponse::get_file_response( 200, "pages/assets/abarrio.jpeg", client_it->second.get_cookie(), offset );
		// 	offset = data.first;
		// 	response = data.second;

		// 	std::cout << "RESPONSE:\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n" << response << "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
		// 	send(fd, response.c_str(), response.size(), 0);
		// 	std::cout << "Response sent" << std::endl;
		// } while (offset != 0);

	/* DEBUGGING: send a response, to close the request and dont make the client wait */
	// response = HTTPResponse::get_autoindex_response( "." + request->get_path(), client_it->second.get_cookie() );
	// std::cout << "RESPONSE:\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n" << response << "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
	// send(fd, response.c_str(), response.size(), 0);

	/* Delete the request data */
	delete request;
	client_it->second.set_request(NULL);
	return false;
}

void	process_requests(std::vector<Server> servers_vector)
{
	/* Storage variables */
	std::map<int, Server> servers;
	std::map<int, Client> clients;
    std::vector<pollfd> pollfds;

	/* NOTE: Init the maps */
	clients.clear();
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
		std::cout << CYAN << "\n[ POLL ]" << RESET << " Waiting for changes...\n\t- Servers: " << servers.size() << "\n\t- Clients: " << clients.size() << std::endl;
		res = poll(&pollfds[0], pollfds.size(), -1);
		if (res < 0)
		{
			if (!sigint_signal)
				std::cout << "Error on the poll function" << std::endl;
			break ;
		}
		/* TODO: Check if timeout happens -> CGI */

		/* Check what fd has changed */
		for (std::vector<pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); it++)
		{
			/* Check if the current fd hsa been modified */
			if ((*it).revents & POLLIN)
			{
				/* Handle the client requests */
				if (handle_clients_request( (*it).fd, clients ))
				{
					pollfds.erase(it);
					break ;
				}

				/* Handle the new connections */
				if (handle_new_connection((*it).fd, servers, clients, pollfds))
					break ;
				/* TODO: handle CGI */
			}
		}
	}

	/* NOTE: Close the client fds, sending a close response */
	std::string response;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		response = HTTPResponse::get_close_connection_template(it->second.get_cookie());
		send(it->first, response.c_str(), response.size(), 0);
		close(it->first);
	}
	/* TODO: Close the CGIs pipes */
}
