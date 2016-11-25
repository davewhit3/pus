#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>

#define MESSAGE_SIZE 256

int main(int argc, char** argv) {
    int sockfd;
    char message[MESSAGE_SIZE];

    struct sockaddr_in6 remote_addr;
    socklen_t addr_len;
    
    if (argc < 3) {
        fprintf(stderr, "Bad arguments!\nusage: %s <ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Utworzenie gniazda dla protokolu TCP: */
    sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) {
        perror("Binding error.");
        exit(EXIT_FAILURE);
    }
    
    //czyszczenie i ustawianie struktury polaczenia
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin6_family = AF_INET6; //ustawnienie typu protokolu ip
    remote_addr.sin6_port = htons(atoi(argv[2]));
    remote_addr.sin6_scope_id = scope_id;
    inet_pton(AF_INET6, argv[1], &remote_addr.sin6_addr);
    addr_len = sizeof(remote_addr);
    
    //skojarzenie deskryptora z gniazdem
    if (connect(sockfd, (const struct sockaddr*) &remote_addr, addr_len) == -1) {
        perror("Connected failed.");
        exit(EXIT_FAILURE);
    }
    
    //czyszczenie bufora messagei i odbieranie danych
    memset(message, 0, MESSAGE_SIZE);
    recv(sockfd, message, MESSAGE_SIZE, 0);
    printf("Message from server: %s\n", message);
    sleep(1);
    
    /* Zamkniecie polaczenia TCP: */
    close(sockfd);
    printf("Connection close.\n");

    return 0;
}
