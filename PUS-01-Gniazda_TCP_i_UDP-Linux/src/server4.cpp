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
		memcpy(originalPort, "1234", 4*sizeof(char));


    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; //tcp
    hints.ai_flags = AI_PASSIVE;     //use local-host address

    if ((errno = getaddrinfo(ipAddress, originalPort, &hints, &servinfo)) != 0) {
       fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errno));
       exit(EXIT_FAILURE);
    }

	strcpy(message, "Welcome to the Chat.\n\r" );

	if ((master_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) {
		perror("Create master_socket");
		exit(EXIT_FAILURE);
    }

	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int))<0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(originalPort));

    unlink(ipAddress);
    if(bind (master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
       printf("\nBind error %d", errno); exit(1);
    }

    freeaddrinfo(servinfo);

	if (listen(master_socket, max_clients)<0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

	while (TRUE) {
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		for (loop1 = 0; loop1 < max_clients; loop1++)
		{
			if (client_socket[loop1] > 0)
			{
				FD_SET(client_socket[loop1], &readfds);
			}
		}

		activity=select(max_clients*2, &readfds, NULL, NULL, NULL);

		if (FD_ISSET(master_socket, &readfds))
		{
			addrlen=sizeof(address);
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

            inet_ntop(AF_INET, &(address.sin_addr), ipConnectingClient, 32);

			printf("Socket #%d [ip=%s, port=%d]\n",new_socket, ipConnectingClient, ntohs(address.sin_port));

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
					buffer[activity] = 0;
					for (loop2 = 0; loop2 < max_clients; loop2++)
					{
						if (loop2 != loop1) {
							send(client_socket[loop2], buffer, strlen(buffer), 0);
						}
					}
				}
			}
		}

	}

	for (loop1=0; loop1 < max_clients; loop1++)
		close(client_socket[loop1]);

	close(master_socket);

	return 0;
}
