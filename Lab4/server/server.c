#include "server.h"

struct sockaddr_in server_addr, client_addr;
int mysock, csock; // socket descriptors
int r, len, n;     // help variables
int server_init()
{
    printf("================== server init ======================\n");
    // create a TCP socket by socket() syscall
    printf("1 : create a TCP STREAM socket\n");
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    if (mysock < 0)
    {
        printf("socket call failed\n");
        exit(1);
    }
    printf("2 : fill server_addr with host IP and PORT# info\n");
    // initialize the server_addr structure
    server_addr.sin_family = AF_INET;                // for TCP/IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // This HOST IP
    server_addr.sin_port = htons(SERVER_PORT);       // port number 1234
    printf("3 : bind socket to server address\n");
    r = bind(mysock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (r < 0)
    {
        printf("bind failed\n");
        exit(3);
    }
    printf(" hostname = %s port = %d\n", SERVER_HOST, SERVER_PORT);
    printf("4 : server is listening ....\n");
    listen(mysock, 5); // queue length = 5
    printf("=================== init done =======================\n");
}