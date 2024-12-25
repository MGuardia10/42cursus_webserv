#include "../../include/process_requests.hpp"
#include "../../include/Client.hpp"
#include "../../include/signals.hpp"
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include "../../include/colors.hpp"

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
	std::map<int, Client>::iterator client_it = clients.find(fd);
	if (client_it == clients.end())
		return false;
	std::cout << YELLOW << "[ NEW REQUEST ]" << RESET << " New request by " << fd;

	/* SECTION: fast implementation to read all the request */
	char buffer[1025];
	std::string header = "";
	size_t	headers_end = std::string::npos;
	int bytes_received;

	while (headers_end == std::string::npos)
	{
		bytes_received = recv(fd, buffer, 1024, 0);

		/* Check if the connection has to be closed */
		if (bytes_received == 0)
		{
			std::cout <<std::endl << RED"[ CLOSED CONNECTION ]" << RESET << " The connection with " << fd << " has been closed" << std::endl;
			clients.erase(client_it);
			close(fd);
			return true;
		}

		buffer[bytes_received] = '\0';
		header += buffer;

		headers_end = header.find("\r\n\r\n");
	}
	
	std::cout << ". Header:\n--------------------------\n" << header << std::endl;
	std::cout << "--------------------------" << std::endl;

	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		// "Connection: keep-alive\r\n"
		"Set-Cookie: " + client_it->second.get_cookie() + "\r\n"
		"Content-Length: 13\r\n"
		"\r\n"
		"Hello World!\n";
	send(fd, response.c_str(), response.size(), 0);
	/* !SECTION */
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

	/* NOTE: Close the client fds */
	/* FIXME: maybe a close header should be sent -> Response module */
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		close(it->first);
	/* TODO: Close the CGIs pipes */
}
