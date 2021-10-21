#include "client.h"

struct sockaddr_in server_addr;
int sock, r;
int client_init()
{
    printf("======= clinet init ==========\n");
    printf("1 : create a TCP socket\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("socket call failed\n");
        exit(1);
    }
    printf("2 : fill server_addr with server’s IP and PORT#\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
    server_addr.sin_port = htons(SERVER_PORT);       // server port number
    printf("3 : connecting to server ....\n");
    r = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (r < 0)
    {
        printf("connect failed\n");
        exit(3);
    }
    printf("4 : connected OK to\n");
    printf("-------------------------------------------------------\n");
    printf("Server hostname=%s PORT=%d\n", SERVER_HOST, SERVER_PORT);
    printf("-------------------------------------------------------\n");
    printf("========= init done ==========\n");
}