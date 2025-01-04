#include <iostream>
#include <vector>
#include "../../include/Server.hpp"
#include "../../include/signals.hpp"
#include "../../include/parse.hpp"
#include "../../include/process_requests.hpp"
#include "../../include/HTTPResponse.hpp"

int	help(char *cmd)
{
	std::cout <<
		"Usage: " << cmd << " <CONFIG_FILE>"
	<< std::endl;
	return (1);
}

int main(int argc, char *argv[])
{
	std::vector<Server>	servers;

	/* Check the params number */
	if (argc != 2)
		return help(argv[0]);

	/* Assign the necessary signals (SIGINT) */
	assign_signals();

	/* Read some config files */
	if (!HTTPResponse::load_data())
	{
		std::cerr << "[ ERROR ] Error reading some configuration files" << std::endl;
		return (1);
	}

	/* Get the servers vector */
	servers = parse(argv[1]);
	if (servers.empty())
		return (1);

	/* DEBUGGING: print the servers data*/
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		std::cout << *it << std::endl;

	/* If the vectors are valid, run them */
	try
	{
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
			it->run();
	}
	catch (Server::ServerException& e)
	{
		std::cout << "Error while running a server: " << e.what() << std::endl;

		/* Close the started servers */
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end() && (*it).is_running(); it++)
			(*it).stop();
		return (1);
	}

	/* Loop (until SIGINT) to detect the servers connections */
	process_requests(servers);

	/* Stop the servers and free the ports */
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		it->stop();

	return (0);
}
