#include "client.h"
#include "remote.h"
#include "local.h"

int main()
{
    char line[MAX];
    char line_copy[MAX];
    char *cmd;
    const int NUM_CMDS = 16;
    char *cmds[] = {"quit", "lrm", "lmkdir", "lrmdir", "lpwd", "lcd", "lls", "lcat", "rm", "mkdir", "rmdir", "pwd", "cd", "ls", "put", "get"};
    void *func[] = {NULL, local_rm, local_mkdir, local_rmdir, local_pwd, local_cd, local_ls, local_cat, remote_rm, remote_mkdir, remote_rmdir, remote_pwd, remote_cd, remote_ls, remote_put, remote_get};
    client_init();
    printf("******** processing loop *********\n");
    while (1)
    {
        printf("input a line : ");
        bzero(line, MAX);           // zero out line[ ]
        fgets(line, MAX, stdin);    // get a line from stdin
        line[strlen(line) - 1] = 0; // kill \n at end
        if (line[0] == 0)           // exit if NULL line
            exit(0);
        strcpy(line_copy, line);
        cmd = strtok(line_copy, " ");
        printf("cmd = %s\n", cmd);
        int (*cmd_func)(char *) = NULL;
        for (int i = 0; i < NUM_CMDS; i++)
        {
            if (!strcmp(cmds[i], cmd))
            {
                cmd_func = func[i];
                if (!cmd_func)
                {
                    exit(0);
                }
                int r = (*cmd_func)(line);
                break;
            }
        }

    }
}