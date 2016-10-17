/**httpserver.c**/
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "string.h"
#include "netinet/in.h"
#include "time.h"
#include "dirent.h"
#include "netdb.h"
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include "arpa/inet.h"

#define BUF_SIZE 409600
#define CLADDR_LEN 100

int createSocket(char * host, int port);
int listenForRequest(int sockfd);
char *getFileType(char * file);

void checkRcev(int status);

int main(int argc, char **argv) { 
 DIR * dirptr;
 FILE * fileptr;
 time_t timenow;
 struct tm * timeinfo;
 time (&timenow);
 timeinfo = localtime(&timenow);

 char *header, *request, *path, *newpath, *host;
 char *dir, *temp;
 int port, sockfd, connfd;
 char get[3], http[9];
 char filepath[BUF_SIZE];
 char http_not_found[] = "HTTP/1.0 404 Not Found\n";
 char http_ok[] = "HTTP/1.0 200 OK\n";
 char buffer[BUF_SIZE];
 char * contentType;

 if (argc != 4) {
  printf("usage: [host] [directory] [portnumber]\n");
  exit(1);
 }

 header = (char*)malloc(BUF_SIZE*sizeof(char));
 request = (char*)malloc(BUF_SIZE*sizeof(char));
 path = (char*)malloc(BUF_SIZE*sizeof(char));
 newpath = (char*)malloc(BUF_SIZE*sizeof(char));

 host = argv[1];
 dir = argv[2];
 port = atoi(argv[3]);

 if ((dirptr = opendir(dir)) == NULL) {
      printf("Directory Not Found!\n");
      exit(1);
 }
 printf("Listing on %s:%d with directory [%s]\n", host, port, dir);
 
 sockfd = createSocket(host, port);
 
 for (;;) {
  printf("--------------------------------------------------------\n");
  printf("Waiting for a connection...\n");
  connfd = listenForRequest(sockfd);
  //gets the request from the connection
  recv(connfd, request, 100, 0);
  printf("Processing request...\n");
  //parses request
  sscanf(request, "%s %s %s", get, path, http);
  
  newpath = path + 1; //ignores the first slash
  sprintf(filepath,"%s/%s", dir, newpath);
  
  contentType = getFileType(newpath);
  sprintf(header, "Date: %sHostname: %s:%d\nLocation: %s\nContent-Type: %s\n\n", asctime(timeinfo), host, port, newpath, contentType);
  
  if ((fileptr = fopen(filepath, "r")) == NULL ) {
    printf("File not found!\n");
    send(connfd, http_not_found, strlen(http_not_found), 0); //sends HTTP 404
  } else {
    printf("Sending the file...[%s]\n", filepath);
    send(connfd, http_ok, strlen(http_ok), 0); //sends HTTP 200 OK  
    checkRcev(recv(connfd, buffer, BUF_SIZE, 0));
    
    if ((temp = strstr(buffer, "User-Agent")) == NULL) {
        printf("Operation aborted by the user!\n");
        break;
    }

   printf("Buffer:\n%s" ,buffer);

   send(connfd, header, strlen(header), 0); //sends the header
  
/*   checkRcev(recv(connfd, buffer, BUF_SIZE, 0));
   printf("Buffer:\n%s" ,buffer);
*/   

   if ((temp = strstr(buffer, "User-Agent")) == NULL) {
    printf("Operation aborted by the user!\n");
    break;
   }
   memset(&buffer, 0, sizeof(buffer));
   while (!feof(fileptr)) { //sends the file
    fread(&buffer, sizeof(buffer), 1, fileptr);
    send(connfd, buffer, sizeof(buffer), 0);
    memset(&buffer, 0, sizeof(buffer));
   }
   printf("File sent...\n");
  }
  printf("Processing completed...\n");
  close(connfd);
 }

 close(sockfd);
 free(header);
 free(request);
 free(path);
 free(newpath);

 return 0;
}

int createSocket(char * host, int port) {
 int sockfd;
 struct sockaddr_in addr;
 struct hostent * host_ent;
 char * hostAddr;

 memset(&addr, 0, sizeof(addr));
 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = htons((short)port);

 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if (sockfd < 0) {  
      printf("Error creating socket!\n");  
      exit(1);  
 }  
 printf("Socket created...\n");

 if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      printf("Error binding socket to port!\n");  
      exit(1);
 }
 printf("Binding done...\n");

 return sockfd;
}

int listenForRequest(int sockfd) {
 int conn;
 char hostip[32];
 struct sockaddr_in addr;
 struct hostent * host;
 struct in_addr inAddr;
 int len;

 addr.sin_family = AF_INET;

 listen(sockfd, 5); //maximum 5 connections
 len = sizeof(addr); 
 if ((conn = accept(sockfd, (struct sockaddr *)&addr, &len)) < 0) {
      printf("Error accepting connection!\n");
      exit(1);
 }
 printf("Connection accepted...\n");
  
 inet_ntop(AF_INET, &(addr.sin_addr), hostip, 32);
 inet_pton(AF_INET, hostip, &inAddr);
 host = gethostbyaddr(&inAddr, sizeof(inAddr), AF_INET);

 printf("---Connection received from: %s [IP= %s]---\n", host->h_name, hostip);
 return conn;
}

char * getFileType(char * file) {
 char * temp;
 if ((temp = strstr(file, ".html")) != NULL) {
  return "text/html";
 } else if ((temp = strstr(file, ".pdf")) != NULL) {
  return "application/pdf";
 } else if ((temp = strstr(file, ".txt")) != NULL) {
  return "text/html";
 }
}

void checkRcev(int recv_size) {
    printf("Recv ");

    if (recv_size == -1){
        printf("error: %d\n", recv_size);
    } else if (recv_size == 0) {
        printf("disconnected: %d\n", recv_size);
    } else  {
        printf("success: %d\n", recv_size);
    }
}