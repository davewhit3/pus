
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h> 

int main(int argc, char** argv) {
    
    int sockfd;
    int retval;
    int new_socket;

    char*           message = "<test message>";
    char            adresIPv4[INET_ADDRSTRLEN];
    
    socklen_t client_addr_len, server_addr_len;
    struct sockaddr_in client_addr, server_addr;

    if (argc != 2) {
        fprintf(stderr, "Bad arguments!\nusage: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    
    //czyszczenie i ustawianie struktury 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;//IPv4
    server_addr.sin_port = htons(atoi(argv[1])); //port
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //wildcard domenowy
    server_addr_len = sizeof(server_addr); 
    
    //polaczenie z gniazdem
    retval = bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len);
    if ( retval == -1) {
        perror("Binding error.");
        exit(EXIT_FAILURE);
    }
    
    printf("Listing...\n");
    client_addr_len = sizeof(client_addr);
    
    // ustawnienie maksymalnej kolejki
    if (listen(sockfd, SOMAXCONN) == -1) {
        perror("Call rejected.");
        exit(EXIT_FAILURE);
    }
    
    for(;;){
        
        if ((new_socket = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len)) < 0){
            perror("Connection error");
            exit(EXIT_FAILURE);
        }else{
            inet_ntop(AF_INET, &client_addr.sin_addr, adresIPv4, INET_ADDRSTRLEN );
            printf("Addr [%s] Port [%d] \n",adresIPv4, ntohs(client_addr.sin_port));

            //wysylanie wiadomosci
            send( new_socket, message, strlen(message), 0);
        }
        close(new_socket);
    }
}
