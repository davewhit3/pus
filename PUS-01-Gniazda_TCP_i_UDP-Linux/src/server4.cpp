#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include "arpa/inet.h"

using namespace std;

#define TRUE   1
#define FALSE  0

int main(int argc, char *argv[])
{
	// variables declaration
	int master_socket, addrlen, new_socket, bufsize = 1025, reuse=TRUE,
		max_clients = 6, activity, loop1, loop2, errno;
    struct addrinfo hints, *servinfo;
	int *client_socket = (int *) malloc (max_clients * sizeof(int));
	struct sockaddr_in address;
	char *message= (char*) malloc (26 * sizeof(char)),
		 *buffer = (char*) malloc(bufsize * sizeof(char)),
		 *originalPort = (char*) malloc(4 * sizeof(char)),
		 *ipAddress;
    char ipConnectingClient[32];

	fd_set readfds;

    if (argc > 1)
		ipAddress = argv[1];
	else
		memcpy(ipAddress, "127.0.0.1", 9*sizeof(char));


	if (argc > 2)
		memcpy(originalPort, argv[2], 4*sizeof(char));
	else
		memcpy(originalPort, "1234", 4*sizeof(char)); // default port

	//socket infoS
    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; //tcp
    hints.ai_flags = AI_PASSIVE;     //use local-host address

    //get server info, put into servinfo
    if ((errno = getaddrinfo(ipAddress, originalPort, &hints, &servinfo)) != 0) {
       fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errno));
       exit(EXIT_FAILURE);
    }

	// message to send to each participant
	strcpy(message, "Welcome to the Chat.\n\r" );

	/// socket() returns a socket descriptor which can be used in other network commands
	if ((master_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) {
		perror("Create master_socket");
		exit(EXIT_FAILURE);
    }

	/// set master socket to allow multiple connections
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int))<0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

	/// Bind the socket to a an Address/Port
	/// type of socket created in socket()
	/// Port to be used for connections
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(originalPort));

	/// unlink and bind
    unlink(ipAddress);
    if(bind (master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
       printf("\nBind error %d", errno); exit(1);
    }

    freeaddrinfo(servinfo);

	/* try to specify maximum of 3 pending connections for the master socket */
	if (listen(master_socket, max_clients)<0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

	while (TRUE) {
		FD_ZERO(&readfds);
		/// add a given file descriptor to a set
		FD_SET(master_socket, &readfds);
		/* reason we say max_clients+3 is stdin,stdout,stderr take up the first
		 * couple of descriptors and we might as well allow a couple of extra.
		 * If your program opens files at all you will want to allow enough extra.
		 * Another option is to specify the maximum your operating system allows.
		 */

		 /* setup which sockets to listen on */

		for (loop1 = 0; loop1 < max_clients; loop1++)
		{
			if (client_socket[loop1] > 0)
			{
				FD_SET(client_socket[loop1], &readfds);
			}
		}

		/// wait for connection, forever if we have to
		activity=select(max_clients*2, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
		/* there was an error with select() */
		}

		/// accept new socket and assign it to the pool of sockets
		if (FD_ISSET(master_socket, &readfds))
		{	/// Open the new socket as 'new_socket'
			addrlen=sizeof(address);
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

            inet_ntop(AF_INET, &(address.sin_addr), ipConnectingClient, 32);

			printf("Socket #%d [ip=%s, port=%d]\n",new_socket, ipConnectingClient, ntohs(address.sin_port));

			/// transmit message to new connection
			/// if send failed to send all the message, display error and exit
			if ((unsigned int)send(new_socket, message, strlen(message), 0) != strlen(message))
				perror("send");

			puts("Welcome message sent successfully");
			for (loop1=0; loop1 < max_clients; loop1++)
			{
				if (client_socket[loop1] == 0)
				{
					client_socket[loop1] = new_socket;
					printf("Adding to list of sockets as %d\n", loop1);
					loop1 = max_clients;
				}
			}
		}
		/*
		 * use read() to read from the socket into a buffer using something
		 * similar to the following:
		 *
		 * If the read didn't cause an error then send buffer to all
		 * client sockets in the array - use for loop and send() just
		 * as if you were sending it to one connection
		 *
		 * important point to note is that if the connection is char-by-char
		 * the person will not be able to send a complete string and you will
		 * need to use buffers for each connection, checking for overflows and
		 * new line characters (\n or \r)
		 */
		for (loop1=0; loop1 < max_clients; loop1++)
		{
			if (FD_ISSET(client_socket[loop1], &readfds))
			{
				if ((activity = read(client_socket[loop1], buffer, 1024)) < 0)
				{
					close(client_socket[loop1]);
					client_socket[loop1] = 0;
				} else
				{
					/// set the terminating NULL byte on the end of the data read
					buffer[activity] = 0;
					for (loop2 = 0; loop2 < max_clients; loop2++)
					{
						/// note, flags for send() are normally 0, see man page for details
						// this if avoids the sender to receive its own message
						if (loop2 != loop1)
							send(client_socket[loop2], buffer, strlen(buffer), 0);
					}
				}
			}
		}

	}

	/// shutting down the sockets properly
	for (loop1=0; loop1 < max_clients; loop1++)
		close(client_socket[loop1]);

	close(master_socket);

	return 0;
}
