#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define MYPORT 12345;


const char *ip6str = "::2";
struct in6_addr result;

bool v6_support();
bool validate_input(int argc, char** argv);

struct in6_addr {
    union {
        uint8_t u6_addr8[16];
        uint16_t u6_addr16[8];
        uint32_t u6_addr32[4];
    } in6_u;

    #define s6_addr                 in6_u.u6_addr8
    #define s6_addr16               in6_u.u6_addr16
    #define s6_addr32               in6_u.u6_addr32
};

struct sockaddr_in6 {
    sa_family_t sin6_family;    /* AF_INET6 */
    in_port_t sin6_port;        /* Transport layer port # */
    uint32_t sin6_flowinfo;     /* IPv6 flow information */
    struct in6_addr sin6_addr;  /* IPv6 address */
    uint32_t sin6_scope_id;     /* IPv6 scope-id */
};

int main(int argc, char** argv) {

    if (!validate_input(&argc, argv)) {
        printf("IPv6 addres is invalid!");
    }


    if (!v6_support()) {
        printf("IPv6 not supported!");
        exit(1);
    }


    struct sockaddr_in6 addr;
    char straddr[INET6_ADDRSTRLEN];

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;        // family
    addr.sin6_port = htons(MYPORT);    // port, networt byte order

    /*
       from presentation to binary representation
    */
    inet_pton(AF_INET6, "2001:720:1500:1::a100", &(addr.sin6_addr));

    /*
       from binary representation to presentation
    */
    inet_ntop(AF_INET6, &addr.sin6_addr, straddr,  sizeof(straddr));


}

bool validate_input(int argc, char argv) {
    if (argc < 1) {
        printf("Usage:\n udpv6 < IPv6 >\n");

        exit(1);
    }
    return false;
}

bool v6_support() {
   return (inet_pton(AF_INET6, ip6str, &result) == 1);
}
