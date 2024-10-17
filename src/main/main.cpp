#include <iostream>
#include <vector>
#include "../../include/Server.hpp"
#include "../../include/signals.hpp"
#include "../../include/parse.hpp"

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

	/* Get the servers vector */
	servers = parse(argv[1]);
	if (servers.empty())
		return (1);

	/* If the vectors are valid, run them */
	/* TODO: Check errors with exceptions? */
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		it->run();

	/* TODO: Loop (until SIGINT) to detect the servers connections */

	/* Stop the servers and free the ports */
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		it->stop();

	return (0);
}
