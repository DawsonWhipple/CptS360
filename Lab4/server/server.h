#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <netdb.h>
#define MAX 256
#define SERVER_HOST "localhost"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1234

extern struct sockaddr_in server_addr, client_addr;
extern int mysock, csock; // socket descriptors
extern int r, len, n;     // help variables

int server_init();


#endif