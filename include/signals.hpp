#pragma once

/* Variable that detects if the ctrl+C signal is recieved */
extern volatile bool	sigint_signal;

/* Function to assign the neccessary signals */
void	assign_signals( void );
