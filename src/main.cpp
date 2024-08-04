#include <iostream>
#include <signal.h>
#include "Server.hpp"

bool	run = true;

void	handle_SIGINT( int sig )
{
	(void) sig;
	run = false;
}

bool	handle_signals( void )
{
	struct sigaction	sa;

	sa.sa_handler = handle_SIGINT;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	return (sigaction(SIGINT, &sa, NULL) != -1);
}

int main( void )
{
	Server	*server;

	/* Assign signals */
	if (!handle_signals())
	{
		std::cout << "Error handling signals (?)" << std::endl;
		return (1);
	}

	/* Init the server */
	try
	{
		server = new Server("Test", 8080, 10);
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

	delete server;
	return (0);
}
