/*
 * Data:                2016-12-16
 * Autor:               Dawid Biały
 * Kompilacja:          $ gcc -Wl,--no-as-needed -lcap capserver.c  -o capserver && sudo setcap cap_net_bind_service=p capserver
 * Uruchamianie:        $ ./capserver 123
 */

#define _GNU_SOURCE     /* getresgid() */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include <sys/capability.h>
#include <sys/types.h>


int main(int argc, char** argv) {

    /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
    int             listenfd, connfd;

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len, server_addr_len;

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];

    unsigned int    port_number;

    uid_t           ruid, euid, suid; /* Identyfikatory uzytkownika. */

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct {
		const char *str;
		cap_flag_t flag;
	} flags[3] = {
		{"EFFECTIVE", CAP_EFFECTIVE},
		{"PERMITTED", CAP_PERMITTED},
		{"INHERITABLE", CAP_INHERITABLE}
	};


    port_number = atoi(argv[1]);


    cap_t cap;
    cap = cap_get_proc();
    if (cap == NULL) {
		perror("cap_get_pid");
		exit(-1);
	}


    cap_value_t cap_list[1];
    cap_list[0] = CAP_NET_BIND_SERVICE;
    cap_flag_value_t cap_flags_value;
    const char *cap_name[CAP_LAST_CAP+1] = {
		"cap_net_bind_service"
	};

    cap_from_name(cap_name[0],  &cap_list[0]);
    cap_get_flag(cap, cap_list[0], flags[1].flag, &cap_flags_value);
    if (cap_flags_value != CAP_SET) {
        perror("You don't have permission - please set CAP_NET_BIND_SERVICE");
		cap_free(cap);
		exit(1);
    }


    /* Utworzenie gniazda dla protokolu TCP: */
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej serwera: */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    server_addr.sin_family          =       AF_INET;
    /* Adres nieokreslony (ang. wildcard address): */
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    /* Numer portu: */
    server_addr.sin_port            =       htons(port_number);
    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_addr_len                 =       sizeof(server_addr);



    /* Pobranie identyfikatorow uzytkownika (real, effective, save set). */
    if (getresuid(&ruid, &euid, &suid) == -1) {
        perror("getresgid()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "UID: %u, EUID: %u, SUID: %u\n", ruid, euid, suid);

    fprintf(stdout, "Binding to port %u...\n", port_number);
    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(listenfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }


    if (getresuid(&ruid, &euid, &suid) == -1) {
        perror("getresgid()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "UID: %u, EUID: %u, SUID: %u\n", ruid, euid, suid);

    /* Program powinien wykonywac sie dalej z EUID != 0 i saved set-user-ID != 0. */
    if (euid == 0) {
        fprintf(stderr, "Run server as unprivileged user!\n");
        exit(EXIT_FAILURE);
    }




    /* Przeksztalcenie gniazda w gniazdo nasluchujace: */
    if (listen(listenfd, 2) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server is listening for incoming connection...\n");

    /* Funkcja pobiera polaczenie z kolejki polaczen oczekujacych na zaakceptowanie
     * i zwraca deskryptor dla gniazda polaczonego: */
    client_addr_len = sizeof(client_addr);
    connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (connfd == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
    }

    fprintf(
        stdout, "TCP connection accepted from %s:%d\n",
        inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
        ntohs(client_addr.sin_port)
    );

    sleep(5);

    fprintf(stdout, "Closing connected socket (sending FIN to client)...\n");
    close(connfd);

    close(listenfd);

    exit(EXIT_SUCCESS);
}
