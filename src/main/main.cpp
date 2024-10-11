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
	
	return (0);
}
