#include <iostream>
#include "../../include/signals.hpp"

int	help(char *cmd)
{
	std::cout <<
		"Usage: " << cmd << " <CONFIG_FILE>"
	<< std::endl;
	return (1);
}

int main(int argc, char *argv[])
{
	/* Check the params number */
	if (argc != 2)
		return help(argv[0]);

	/* Assign the necessary signals (SIGINT) */
	assign_signals();

	/* TODO: Parse -> get the servers vector */
	/* TODO: If the vectors are valid, run them */
	/* TODO: Loop (until SIGINT) to detect the servers connections */
	/* TODO: Stop the servers amd free the ports */

	return (0);
}
