#include <iostream>
#include <csignal>

volatile bool	sigint_signal;

void	sigint_handler( int signal )
{
	(void) signal;
	sigint_signal = true;
}

void	assign_signals( void )
{
	sigint_signal = false;
	signal(SIGINT, sigint_handler);
}
