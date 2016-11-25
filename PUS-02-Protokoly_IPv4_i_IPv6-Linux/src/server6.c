
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 

#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED(a) \
       ((((a)->s6_words[0]) == 0) && \
        (((a)->s6_words[1]) == 0) && \
        (((a)->s6_word[2]) == 0) && \
        (((a)->s6_word[3]) == 0) && \
        (((a)->s6_word[4]) == 0) && \
        (((a)->s6_word[5]) == 0xFFFF))
#endif

int main(int argc, char** argv) {
    int sockfd;
    int retval;
    int new_socket;

    char* message = "<test message>";
    char adresIPv6[INET6_ADDRSTRLEN];
    
    struct sockaddr_in6 client_addr, server_addr;
    socklen_t client_addr_len, server_addr_len; 
    
     if (argc != 2) {
        fprintf(stderr, "Bad arguments!\nusage: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
        
    sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) {
        fprintf(stderr, "Socket binding error.\n");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin6_family = AF_INET6; 
    server_addr.sin6_port = htons(atoi(argv[1]));
    server_addr.sin6_addr = in6addr_any;
    server_addr_len = sizeof(server_addr);

    //polaczenie z gniazdem
    retval = bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len);
    if (retval == -1) {
        perror("Binding error.");
        exit(EXIT_FAILURE);
    }
    
    printf("Listing...\n");
    client_addr_len = sizeof(client_addr);
    
    //ustawienie liczby polaczen w kolejce
    if (listen(sockfd, SOMAXCONN) == -1) { 
        perror("Call rejected.");
        exit(EXIT_FAILURE);
    }
    
    for(;;){
        if ((new_socket = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len)) < 0){
            perror("Connection error");
            exit(EXIT_FAILURE);
        } else {
            
            if(!IN6_IS_ADDR_V4MAPPED(&client_addr.sin6_addr)) {
                printf("Client is v6\n");
            }else {
                printf("Client is v4\n");
            }
            
            inet_ntop(AF_INET6, &client_addr.sin6_addr, adresIPv6, INET6_ADDRSTRLEN );
            printf("Addr [%s] Port [%d] \n",adresIPv6, ntohs(client_addr.sin6_port));
            fflush(stdout);
            
            send( new_socket, message, strlen(message), 0);
        }
        close(new_socket);
    }
}
