#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h> 

#define MESSAGE_SIZE 256

int main(int argc, char** argv)
{
    
    int             sock_stat;
    struct          sockaddr_in remote_addr;
    socklen_t       addr_len;
    char            message[MESSAGE_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Bad arguments!\nusage: %s <IPv4 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Utworzenie gniazda dla protokolu TCP: */
    sock_stat = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_stat == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    
    //czyszczenie struktury
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);
    addr_len        = sizeof(remote_addr);
    
    //skojarzenie deskryptora z gniazdem
    if (connect(sock_stat, (const struct sockaddr*) &remote_addr, addr_len) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }
    
    memset(message, 0, MESSAGE_SIZE);
    recv(sock_stat, message, MESSAGE_SIZE, 0);

    printf("Message from server: %s\n", message);
    sleep(1);
    
    close(sock_stat);
    printf("Connection close.\n");
    
    return 0;
}
