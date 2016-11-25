/*
 * Data:                2009-02-18
 * Autor:               Jakub Gasior <quebes@mars.iti.pk.edu.pl>
 * Kompilacja:          $ gcc ipaddr.c -o ipaddr
 * Uruchamianie:        $ ./ipaddr <adres IPv4 lub IPv6>
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFOR_SIZE 1024
#define IP_V4 4
#define IP_V6 6

char* ipv4_binary_string(uint32_t addr);

int main(int argc, char** argv) {
    
    
    if (argc < 3) {
        fprintf(stderr, "Bad arguments!\nusage: %s <ADDR> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    
    int sock;
    int port = atoi(argv[2]);
    char buff[BUFFOR_SIZE];
    char buff4[INET_ADDRSTRLEN];
    char buff6[INET6_ADDRSTRLEN];

    struct in6_addr adres;
    struct sockaddr* adrp;
    struct sockaddr_in adr;
    struct sockaddr_in6 adr6;
    int sock_storage_size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage sock_storage;
    
    //czyszczenie
    memset(&adr, 0, sizeof(adr));
    memset(&adr6, 0, sizeof(adr6));
    
    struct addrinfo         hints;
    struct addrinfo         *result;

    int                     retval;
    int                     ip_version;
    
    
    char                    address[NI_MAXHOST];
    
    /* Rozmiar sockwej struktury adresowej: */
    socklen_t               sockaddr_size;
    
    memset(&hints, 0, sizeof(hints));
    /* Pozwalamy na AF_INET or AF_INET6: */
    hints.ai_family         =       AF_UNSPEC;
    /* sock typu SOCK_STREAM (TCP): */
    hints.ai_socktype       =       SOCK_STREAM;
    /* Dowolny protokol: */
    hints.ai_protocol       =       0;
    

    if ((retval = getaddrinfo(argv[1], NULL, &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }
    
    //utworzenie gniazda
    sock = socket(result->ai_family, SOCK_STREAM, 0);
    if (result->ai_family == AF_INET) {
        ip_version = IP_V4;
        sockaddr_size = sizeof(struct sockaddr_in);
        
        adr.sin_family = AF_INET;
        adr.sin_port = htons(port);
        adr.sin_addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr;
        
        adrp = (struct sockaddr *)&adr;
        
    } else {
        ip_version = IP_V6;
        sockaddr_size = sizeof(struct sockaddr_in6);
        
        adr6.sin6_family = AF_INET6;
        adr6.sin6_port = htons(port);
        adr6.sin6_addr = ((struct sockaddr_in6 *) result->ai_addr)->sin6_addr;
        
        adrp = (struct sockaddr *)&adr6;
    }
    
    
    
    if (connect(sock, adrp, sockaddr_size) < 0) {
        fprintf(stderr, "Bad connected.\n");
        exit(EXIT_FAILURE);
    }
    
    
    printf("Connected.\n");
    
    // Wypisuje IP
    retval = getnameinfo(
                         (struct sockaddr *)result->ai_addr,
                         sockaddr_size,
                         address, NI_MAXHOST,
                         NULL, 0,
                         NI_NUMERICHOST
                         );
    
    if (retval != 0) {
        fprintf(stderr, "getnameinfo(): %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }
    
    printf("[IPv%d]  %s\n", ip_version, address);
    
    
    // Wypisuje dane z socketa
    getsockname(sock, (struct sockaddr *) &sock_storage, &sock_storage_size);
    if(ip_version == IP_V4) {
        fprintf(stderr, "[sock %s] %d\n", 
            inet_ntop(
                AF_INET,
                &(((struct sockaddr_in *)&sock_storage)->sin_addr), 
                buff4, 
                INET_ADDRSTRLEN
            ),
            ntohs(((struct sockaddr_in *)&sock_storage)->sin_port));
    } else {
        fprintf(stderr, "[sock %s] %d\n", 
            inet_ntop(
                AF_INET6, 
                &(((struct sockaddr_in6 *)&sock_storage)->sin6_addr), 
                buff6, 
                INET6_ADDRSTRLEN
            ),
            ntohs(((struct sockaddr_in6 *)&sock_storage)->sin6_port));
    }
    
    // odbieranie message z serwera
    recv(sock, buff, sizeof(buff), 0);
    printf("Message from server: %s\n", buff);
    
    close(sock);   
    
    // czyszczenie pamieci
    if (result != NULL) {
        freeaddrinfo(result);
    } 
    
    printf("Connection closed\n");
    return 0; 
}
