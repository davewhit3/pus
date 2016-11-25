#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include "libpalindrome.h"

int main(int argc, char** argv) {

    int             sockfd;
    int             retval;
    struct          sockaddr_in client_addr, server_addr;
    socklen_t       client_addr_len, server_addr_len;
    char            buff[256];
    char            addr_buff[256];
    int ispal;

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family          =       AF_INET;
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    server_addr.sin_port            =       htons(atoi(argv[1]));
    server_addr_len                 =       sizeof(server_addr);

    if (bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    while(1) {
        memset(buff, 1, sizeof(buff));
        fprintf(stdout, "Listening for incoming datagram...\n");
        client_addr_len = sizeof(client_addr);
        retval = recvfrom(
            sockfd,
            buff, sizeof(buff),
            0,
            (struct sockaddr*)&client_addr, &client_addr_len
        );

        if (retval == -1) {
            perror("recvfrom()");
            exit(EXIT_FAILURE);
        }    

        fprintf(stdout, "UDP datagram received from %s:%d. Echoing message...\n",
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
            ntohs(client_addr.sin_port)
        );

        sleep(1);

        if (retval == 0) {
            fprintf(stdout, "Empty datagram received, terminating...\n");
            close(sockfd);
            exit(EXIT_SUCCESS);
        }

        fprintf(stdout, "Checking palindrome...\n");

        sleep(1);

        ispal = is_palindrome(buff, retval);
        memset(buff, 1, 255);

        switch(ispal) {
            case -1:
                sprintf(buff, "Data contains non-numerical characters");
                break;
            case 0: 
                sprintf(buff, "Data is not a palindrome");
                break;
            case 1:
                sprintf(buff, "Data is a valid palindrome");
                break;
        }

        fprintf(stdout, "Sending message...\n");

        retval = sendto(
            sockfd,
            buff, 40,
            0,
            (struct sockaddr*)&client_addr, client_addr_len
        );

        if (retval == -1) {
            perror("sendto()");
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }
}
