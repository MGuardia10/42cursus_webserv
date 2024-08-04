#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "Server.hpp"

# define DEFAULT_PORT		8080
# define DEFAULT_BACKLOG	10

bool	run = true;

void	handle_SIGINT( int sig )
{
	(void) sig;
	run = false;
	std::cout << std::endl;
}

bool	handle_signals( void )
{
	struct sigaction	sa;

	sa.sa_handler = handle_SIGINT;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	return (sigaction(SIGINT, &sa, NULL) != -1);
}

int main( int argc, char **argv )
{
	Server	*server;
	int		port;
	int		backlog;

	/* Assign signals */
	if (!handle_signals())
	{
		std::cout << "Error handling signals (?)" << std::endl;
		return (1);
	}

	/* Check server options */
	port = (argc >= 2 ? atol(argv[1]) : DEFAULT_PORT);
	backlog = (argc >= 3 ? atol(argv[2]) : DEFAULT_BACKLOG);

	/* Init the server */
	try
	{
		server = new Server("Test", port, backlog);
		server->start();
	}
	catch (std::exception& e)
	{
		std::cout << "[ ERROR ] " << e.what() << std::endl;
		return (1);
	}

	/* Print the server info and set the loop */
	std::cout << *server << "Listening..." << std::endl;
	while (run)
	{
		try
		{
			server->acceptConnection();
		}
		catch (std::exception& e)
		{
			std::cout << "[ ERROR ] " << e.what() << std::endl;
			run = false;
		}
	}

	/* Delete and close the server */
	delete server;
	return (0);
}
