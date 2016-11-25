#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_pton() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {

    int             sockfd;
    int             retval;
    struct          sockaddr_in remote_addr;
    socklen_t       addr_len;
    char            buff[256];
    int n;

    if (argc != 3) {
        fprintf(
            stderr,
            "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]
        );
        exit(EXIT_FAILURE);
    }

    
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    retval = inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);

    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    remote_addr.sin_port = htons(atoi(argv[2]));
    addr_len = sizeof(remote_addr);

    fprintf(stdout, "Connecting to remote address...\n");

    connect(sockfd, (struct sockaddr*) &remote_addr, addr_len);

    while(1) {
        memset(buff, 0, sizeof(buff));
        fprintf(stdout, "Connected to %s. Input message:\n", argv[1]);

        n = 0;

        do {
            buff[n++] = getchar();
        } while (strcmp(&buff[n-1], "\n\0"));

        if (n == 1) {
            fprintf(stdout, "No data given. Terminating\n");
            send(sockfd, NULL, 0, 0);    
            close(sockfd);
            exit(EXIT_SUCCESS);
        }

        fprintf(stdout, "Sending message to %s...\n", argv[1]);
        retval = send(sockfd, buff, n, 0);

        if (retval == -1) {
            perror("sendto()");
            exit(EXIT_FAILURE);
        }

        memset(buff, 0, sizeof(buff));

        retval = recv(sockfd, buff, sizeof(buff), 0);
    
        if (retval == -1) {
            perror("recvfrom()");
            exit(EXIT_FAILURE);
        }

        buff[retval] = '\0';

        fprintf(stdout, "Server response: '%s'\n", buff);           
    }

    close(sockfd);

    exit(EXIT_SUCCESS);
}
