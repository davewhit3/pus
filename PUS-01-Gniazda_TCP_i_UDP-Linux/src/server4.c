#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>


#define BUFFER_SIZE 1025
#define MSG_LENGTH 50
#define REUSE 1
#define MAX_CLIENTS 5

int main(int argc, char *argv[]) 
{
    int masterSocket;
    int addressLength;
    int incomingSocket;
    int incomingData;
    int index;
    int flags;
    int maxFds;
    int *clients = (int *) malloc(MAX_CLIENTS * sizeof(int));
    struct sockaddr_in addressStruct;
    char *message = (char*) malloc(MSG_LENGTH * sizeof(char));
    char *buffer = (char*) malloc(BUFFER_SIZE * sizeof(char));
    char *port = (char*) malloc(5 * sizeof(char));
    char *ipAddress = (char*) malloc(15 * sizeof(char));
    char clientIp[15];
    fd_set descriptors;
    struct timeval timeout;

    if (argc < 3) {
        fprintf(stderr, "Podaj adres i port!");
        exit(EXIT_FAILURE);
    }

    memcpy(ipAddress, argv[1], strlen(argv[1]) < 15 ? strlen(argv[1]) : 15);
    memcpy(port, argv[2], 5*sizeof(char));
    message = "Server: No siema\n\0";
    memset(clients, 0, MAX_CLIENTS * sizeof(int));
    memset(&addressStruct, 0, sizeof(addressStruct));
    
    if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket()");
        exit(EXIT_FAILURE);
    }

    flags = fcntl(masterSocket, F_GETFL, 0);
    fcntl(masterSocket, F_SETFL, flags | O_NONBLOCK);
    int reuseAddress = 1;

    if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddress, sizeof(reuseAddress)) < 0) {
        fprintf(stderr, "setsockopt");
        exit(EXIT_FAILURE);
    }

    addressStruct.sin_family = AF_INET;
    addressStruct.sin_addr.s_addr = INADDR_ANY;
    addressStruct.sin_port = htons(atoi(port));

    if (bind(masterSocket, (struct sockaddr *) &addressStruct, sizeof(addressStruct)) < 0) {
       fprintf(stderr, "bind()");
       exit(EXIT_FAILURE);
    }

    if (listen(masterSocket, MAX_CLIENTS) < 0) {
        fprintf(stderr, "listen()");
        exit(EXIT_FAILURE);
    }

    addressLength = sizeof(addressStruct);
    maxFds = masterSocket;

    while (1) {
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        FD_ZERO(&descriptors);
        FD_SET(masterSocket, &descriptors);

        for (index = 0; index < MAX_CLIENTS; index++) {
            if (clients[index] > 0) {
                FD_SET(clients[index], &descriptors);

                if (clients[index] > maxFds) {
                    maxFds = clients[index];
                }
            }
        }

        if ((incomingData = select(maxFds + 1, &descriptors, NULL, NULL, NULL)) < 0) {
            fprintf(stderr, "Select error: %d %d\n", errno, errno == EBADF);
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(masterSocket, &descriptors)) {
            incomingSocket = accept(masterSocket, (struct sockaddr *) &addressStruct, (socklen_t*) &addressLength);

            if (incomingSocket > 0) {
                inet_ntop(AF_INET, &(addressStruct.sin_addr), clientIp, INET6_ADDRSTRLEN);
                fprintf(stderr, "Zaakceptowane połączenie! IP: %s, port: %d\n", clientIp, ntohs(addressStruct.sin_port));

                flags = fcntl(incomingSocket, F_GETFL, 0);
                fcntl(incomingSocket, F_SETFL, flags | O_NONBLOCK);

                if ((unsigned int) send(incomingSocket, message, strlen(message), 0) != strlen(message)) {
                    fprintf(stderr, "send()");
                }

                for (index = 0; index < MAX_CLIENTS; index++) {
                    if (clients[index] == 0) {
                        clients[index] = incomingSocket;
                        break;
                    }
                }
            } else if (errno != EWOULDBLOCK) {
                fprintf(stderr, "accept(), %d, %d", EWOULDBLOCK, incomingSocket);
                exit(EXIT_FAILURE);
            }
        }

        for (int fd = 0; fd < MAX_CLIENTS; fd++) {
            if (FD_ISSET(clients[fd], &descriptors)) {
                if ((incomingData = recv(clients[fd], buffer, BUFFER_SIZE, 0)) <= 0) {
                    if (incomingData == 0 && errno != EAGAIN) {
                        fprintf(stderr, "Connection closed: %d", fd);
                        FD_CLR(clients[fd], &descriptors);
                        close(clients[fd]);
                        clients[fd] = 0;
                    } else {
                        fprintf(stderr, "read() %d", errno);
                    }
                } else {
                    buffer[incomingData] = '\0';

                    for (index = 0; index < MAX_CLIENTS; index++) {
                        if (index != fd) {
                            fprintf(stderr, "Send %d\n", index);
                            send(clients[index], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }

        sleep(2);
    }

    for (index = 0; index < MAX_CLIENTS; index++) {
        close(clients[index]);
    }

    close(masterSocket);

    return 0;
}
