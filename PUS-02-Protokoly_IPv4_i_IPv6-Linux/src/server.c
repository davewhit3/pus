#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

//mapowanie adresu 
//http://stackoverflow.com/a/14283766
#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED(a) \
((((a)->s6_words[0]) == 0) && \
(((a)->s6_words[1]) == 0) && \
(((a)->s6_word[2]) == 0) && \
(((a)->s6_word[3]) == 0) && \
(((a)->s6_word[4]) == 0) && \
(((a)->s6_word[5]) == 0xFFFF))
#endif
#define BUFFOR_SIZE 1024
char* ipv4_binary_string(uint32_t addr);

int main(int argc, char** argv) {
    char message[BUFFOR_SIZE] = "<test message>";
    char bufor[BUFFOR_SIZE];
    char buff6[INET6_ADDRSTRLEN];
    int sock, sock2;
    
    struct sockaddr_in6 server, client;

    if (argc < 2) {
        fprintf(stderr, "Bad arguments!\nusage: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    unsigned int port = atoi(argv[1]);
    
    
    socklen_t dl = sizeof(struct sockaddr_in6);
    
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(port);
    server.sin6_addr = in6addr_any;
    
    
    if (bind(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
        fprintf(stderr, "Socket binding error.\n");
        return 1;
    }
    
    //ustawianie kolejki polaczen
    if (listen(sock, 10) < 0) {
        fprintf(stderr, "Listen failed.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Listing...\n");
    
    
    while ( (sock2 = accept(sock, (struct sockaddr*) &client, &dl)) > 0 ) {
        printf("Client connected %s:%d",  
                inet_ntop(AF_INET6, &(client.sin6_addr), buff6, INET6_ADDRSTRLEN), client.sin6_port);
        
        if (IN6_IS_ADDR_V4MAPPED(&client.sin6_addr)) {
            printf(" [IPv4 >> IPv6)]\n");
        } else {
            printf(" [IPv6]\n");
        }
        
        printf("Sending message...\n");
        
        //czyszczenie bufora
        memset(&bufor, 0, sizeof(bufor));
        send(sock2, message, sizeof(message), 0);
        close(sock2);
        
        printf("Connection closed.\n");
    }
    
    close(sock); 
}
