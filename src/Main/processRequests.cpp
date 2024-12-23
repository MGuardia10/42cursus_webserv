#include "../../include/processRequests.hpp"
#include "../../include/Client.hpp"
#include "../../include/signals.hpp"
#include <poll.h>

static std::map<int, Server>	vector_to_map( std::vector<Server> servers_vector )
{
	std::map<int, Server>	servers_map;
	servers_map.clear();

	for (std::vector<Server>::iterator servers_it = servers_vector.begin(); servers_it != servers_vector.end(); servers_it++)
	{
		std::vector<int> sockets = (*servers_it).get_sockets();

		for (std::vector<int>::iterator sockets_it = sockets.begin(); sockets_it != sockets.end(); sockets_it++)
			servers_map.insert(std::pair<int, Server>(*sockets_it, *servers_it));
	}

	return servers_map;
}

void	processRequests(std::vector<Server> servers_vector)
{
	/* Storage variables */
	std::map<int, Server> servers;
	std::map<int, Client> clients;
    std::vector<pollfd> fds;

	/* Init the maps */
	
	servers = vector_to_map( servers_vector );
	clients.clear();

	/* Insert all the fds on the pollfd vector */
	fds.clear();
	for (std::map<int, Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		pollfd current_fd;
		current_fd.fd = it->first;
		current_fd.events = POLLIN;
		fds.push_back(current_fd);
	}

	while (!sigint_signal)
	{
		int res = poll(&fds[0], fds.size(), -1);
		
		std::cout << "Changes: " << res << std::endl;
		for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++)
			std::cout << (*it).fd << " => " << (*it).revents << std::endl;

		sigint_signal = true;
	}
}