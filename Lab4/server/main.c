#include "server.h"
#include "commands.h"

int main()
{
    char line[MAX];
    char line_copy[MAX];
    char *cmd;
    const int NUM_CMDS = 8;
    char *cmds[] = {"rm", "mkdir", "rmdir", "pwd", "cd", "ls", "put", "get"};
    void *func[] = {server_rm, server_mkdir, server_rmdir, server_pwd, server_cd, server_ls, server_put, server_get};
    int (*cmd_func)(char *) = NULL;
    server_init();
    while (1)
    { // Try to accept a client request
        printf("server: accepting new connection ....\n");
        // Try to accept a client connection as descriptor newsock
        len = sizeof(client_addr);
        csock = accept(mysock, (struct sockaddr *)&client_addr, &len);
        if (csock < 0)
        {
            printf("server: accept error\n");
            exit(1);
        }
        printf("server: accepted a client connection from\n");
        printf("---------------------------------------------–\n");
        printf("Clinet: IP=%s port=%d\n",
               inet_ntoa(client_addr.sin_addr.s_addr),
               ntohs(client_addr.sin_port));
        printf("---------------------------------------------–\n");
        // Processing loop: client_sock <== data ==> client
        while (1)
        {
            n = read(csock, line, MAX);
            if (n == 0)
            {
                printf("server: client died, server loops\n");
                close(csock);
                break;
            }

            // show the line string
            printf("server: read n=%d bytes; line=%s\n", n, line);

            strcpy(line_copy, line);
            cmd = strtok(line_copy, " ");
            for (int i = 0; i < NUM_CMDS; i++)
            {
                if (!strcmp(cmds[i], cmd))
                {
                    cmd_func = func[i];
                    int r = (*cmd_func)(line);
                    break;
                }
            }
            printf("server: ready for next request\n");
        }
    }
}