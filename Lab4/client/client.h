#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>

#define MAX 256
#define SERVER_HOST "localhost"
#define SERVER_PORT 1234

extern struct sockaddr_in server_addr;
extern int sock, r;

int client_init();


#endif