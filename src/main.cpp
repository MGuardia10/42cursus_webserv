#include <iostream>

int	help(char *cmd)
{
	std::cout <<
		"Usage: " << cmd << " <CONFIG_FILE>"
	<< std::endl;
	return (1);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		return help(argv[0]);
	return (0);
}
