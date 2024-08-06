# Subject functions

1. [Common functions](#1.0)
2. [General utils](#2.0)
	- [hton/ntoh](#2.1)
	- [fcntl](#2.2)
3. [Socket functions](#3.0)
	- [socket](#3.1)
	- [setsockopt](#3.2)
	- [socketpair](#3.3)
	- [getsockname](#3.4)
	- [getprotobyname](#3.5)
4. [Server comunication](#4.0)
	- [Functions](#4.1)
		- [bind](#4.1.1)
		- [listen](#4.1.2)
		- [accept](#4.1.3)
		- [recv](#4.1.4)
		- [send](#4.1.5)
		- [connect](#4.1.6)
	- [Examples](#4.2)
	- [Summary](#4.3)
5. [Network addess and service translation](#5.0)
	- [getaddrinfo](#5.1)
	- [freeaddrinfo](#5.2)
	- [gai_strerror](#5.3)

<br><hr><hr>

## <b id="1.0">COMMON FUNCTIONS</b>
| Tag | Functions|
|-|-|
| Error check | errno, strerror |
| Files interactions | open, close, read, write, access, stat |
| Folder interactions | chdir, opendir, readdir, closedir (and stat) |
| Execute commands | dup, dup2, pipe, fork, execve, waitpid |
| Signals | kill, signal |

<br>

## <b id="2.0">GENERAL UTILS</b>
### <b id="2.1">hton/ntoh</b>
Networks use the "Big Endian" format for data transmission; however, not all processors use this format. The functions `htons`, `htonl`, `ntohs` and `ntohl` are responsible for making the necessary conversions so that the treatment of the variables is correct.

| Function | Initials | Description | Argument type |
|-|-|-|-|
| htons | Host to Network Short | Convert a number to a network byte format. It's often to be used on ports | short |
| htonl | Host to Network Long | Convert a number to a network byte format. It's often to be used on IP adresses | int |
| ntohs | Network to Host Short | Converts a number to network byte format, so that it can be sent. It's often to be used on ports recieved from the network | short |
| ntohl | Network to Host Long | Converts a number to network byte format, so that it can be sent. It's often to be used on IP adresses recieved from the network | int |

Examples:
- Set the server port
```C
/* Data port */
int port = 8080;

/* Usage of the port */
struct sockaddr_in	addr = {
	/* ... */
	.sin_port = htons(_port),
	/* ... */
};
```
- Send int as data
```C
/* Data to send */
int			data = 123;

/* Conversion */
uint32_t	send_data = htonl(data);

/* Send */
send(client_fd, &send_data, sizeof(send_data), 0);
```
- Data receiving
```C
/* Data is received */
uint32_t	rec_data;
recv(socket_fd, &rec_data, sizeof(rec_data), 0);

/* Conversion */
uint32_t	data = ntohl(rec_data);
```
Summary:
![a](img/hton-ntoh.png)


### <b id="2.2"> fcntl</b>
The function `fcntl` let the use to modify a file descriptor configuration. For example, it can be use to set a fd not blocking
```C
/*
 * @brief	Function to modify a fd config
 *
 * @param[in]	fd File descriptor to modify
 * @param[in]	cmd Aspect to change
 * @param[in]	... Value to set
 *
 * @return	The value depends of the cmd
*/
int fcntl(int fd, int cmd, ... /* arg */ );
```

## <b id="3.0">SOCKET FUNCTIONS</b>
A socket is an endpoint for communication, used as file descriptor. There are some functions to create/manage sockets:
- <b id="3.1">`socket`</b>
```C
/*
 * @brief	Creates a scoket
 *
 * @param[in]	domain Specifies a communication domain/mode
 * @param[in]	type Specifies the type of communication (two-way, one-way...)
 * @param[in]	protocol Specifies the protocol that the socket has (usually, 0)
 *
 * @return	File descriptor that referies the socket
*/
int socket(int domain, int type, int protocol);
```
- <b id="3.2">`setsockopt`</b>
```C
/*
 * @brief	Manipulates the socket properties
 * 
 * @param[in]	sockfd Socket to modify
 * @param[in]	level Level to set (on sockets, SOL_SOCKET is used)
 * @param[in]	optname Name of the variable to set
 * @param[in]	optval Value to set
 * @param[in]	optlen Size of the variable setted
 * 
 * @return 0 on success, -1 otherwise
*/
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```
- <b id="3.3">`socketpair`</b>
```C
/*
 * @brief	Creates an unnamed pair of connected sockets
 *
 * @param[in]	domain Sockets domain
 * @param[in]	type Sockets type
 * @param[in]	protocol Sockets protocol
 * @param[out]	sv Array where the sockets will be saved
 *
 * @return	0 on success, -1 otherwise
*/
int socketpair(int domain, int type, int protocol, int sv[2]);
```
- <b id="3.4">`getsockname`</b> 
```C
/*
 * @brief	Function to get the information of a socket
 *
 * @param[in]	sockfd Socket to check
 * @param[out]	addr Variable where the info will be saved
 * @param[out]	addrlen Size of the variable where the data where saved
 *
 * @return 0 on success, -1 otherwise
*/
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- <b id="3.5">`getprotobyname`</b>
```C
/*
 * @brief	Search an entry on the database that matches with the protocol `name`
 *
 * @param[in]	name Protocol to search
 *
 * @return	A pointer to the entry, NULL otherwise
*/
struct protoent *getprotobyname(const char *name);
```


## <b id="4.0">SERVER COMMUNICATION</b>
On `libc` there are functions to interact with sockets, so that communication can occur between two networks.
### <b id="4.1">Functions</b>
- <b id="4.1.1">`bind`</b>
```C
/*
 * @brief	Function to bind a socket to a specific config (ips, ports...)
 *
 * @param[in]	sockfd	Socket to bind
 * @param[in]	addr	Configuration to bind the socket
 * @param[in]	addlen	Length of the addr struct
 * 
 * @return	0 on success, otherwise -1
*/
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
- <b id="4.1.2">`listen`</b>
```C
/*
 * @brief	Function that lets the socket to accept requests
 *
 * @param[in]	sockfd	Socket to listen
 * @param[in]	backlog	Maximum message queue length
 * 
 * @return	0 on success, otherwise -1
*/
int	listen(int sockfd, int backlog);
```
- <b id="4.1.3">`accept`</b>
```C
/*
 * @brief	It extracts the first connection request on the queue of pending
 *			connections for the listening socket
 *
 * @param[in]	sockfd	Socket to check
 * @param[out]	addr	Struct that will be filled with the connection data
 * @param[out]	addrlen	Length of the struct filled
 * 
 * @return	File descriptor of the connection, or -1 on error
*/
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- <b id="4.1.4">`recv`</b>
```C
/*
 * @brief	Function to recieve messages from a socket
 *
 * @param[in]	sockfd	Socket to read
 * @param[out]	buf		Buffer where the information is saved
 * @param[in]	len		Size of the variable
 * @param[in]	flags	Flags that can affect the action (like the
 * 						`read` function)
 * 
 * @return	Number of bytes received, or -1 in error case
*/
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```
- <b id="4.1.5">`send`</b>
```C
/*
 * @brief	Function to send messages through a socket
 *
 * @param[in]	sockfd	Socket where the information is sent
 * @param[in]	buf		Information to send
 * @param[in]	len		Size of the message, in bytes
 * @param[in]	flags	Flags that can be applied (like the `write`
 * 						function)
*/
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```
- <b id="4.1.6">`connect`</b> (function that does not make sense for a server to use, since its purpose is to connect to one)
```C
/*
 * @brief	System call to connect to the socket referred
 *
 * @param[in]	sockfd	Connection socket
 * @param[in]	addr	Information about the connection to do
 * @param[in]	addrlen	Size of the addr struct
 * 
 * @return	0 on success case, -1 otherwise
*/
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### <b id="4.2">Example</b>
```C
int main( void )
{
	/* Socket creation */
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Bind the socket */
	struct sockaddr_in	addr = {
		.sin_family = AF_INET,
		.sin_port = htons(_port),
		.sin_addr = INADDR_ANY,
		.sin_zero = {0}
	};
	bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

	/* Listen to the port */
	listen(sockfd, 10);

	/* Loop of requests */
	int connection;
	while (1)
	{
		connection = accept(sockfd, NULL, NULL);
		if (connection == -1)
			break ;

		/* Read the data */
		char	buffer[257] = {0};
		recv(connection, buffer, 256, 0);

		/* Send data */
		char	*msg = "Hello from server";
		send(connection, msg, strlen(msg), 0);

		/* Close the connection */
		close(connection);
	}

	/* Close the socket fd */
	close(sockfd);
	return (0);
}
```

### <b id="4.3">Summary</b>
![Connection diagram](img/connection-diagram.png)

## <b id="5.0">NETWORK ADDRESS AND SERVICE TRANSLATION</b>
Also, there are functions to get the information of Internet address:
- <b id="5.1">`getaddrinfo`</b>
```C
/*
 * @brief	Function to get an address info
 *
 * @param[in]	node Node of the addresses
 * @param[in]	service Service of the addresses
 * @param[in]	hints Information to get addresses with the same config
 * @param[out]	res Information searched
 *
 * @return	0 on success; otherwise, an error code. The errors can
 			be read by the `gai_strerror` function
*/
int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);
```
- <b id="5.2">`freeaddrinfo`</b>
```C
/*
 * @brief	Function to free the addresses info
 *
 * @param[in]	res Information/struct to free
*/
void freeaddrinfo(struct addrinfo *res);
```
- <b id="5.3">`gai_strerror`</b>
```C
/*
 * @brief	Function to read and error code
 *
 * @param[in]	errcode Error code
 *
 * @return	The string error, or NULL if the error don't exists
*/
const char *gai_strerror(int errcode);
```
