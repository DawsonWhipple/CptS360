/*********************************************************
 * Author: Dawson Whipple
 * Project: CS360 Lab3
 * Date: 9/30/21
 * Description: sh simulator 
 * ******************************************************/
/***** LAB3 base code *****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int tokenize(char *line, int *narg, char *args[]);
void parsePath(char *pathname, char *dir[], int *ndir);
void getCommandLine(int ndir, char *dirs[], char *line, char *cmd);
void setOutRedir(int nargs, char *args[]);
void setInRedir(int nargs, char *args[]);
void pipeWriter(char *line_head, int pd[], int ndir, char *dirs[], char *env[]);
void pipeReader(char *line_tail, int pd[], int ndir, char *dirs[], char *env[]);
void printArgs(int nargs, char *args[]);
int isPipe(int nargs, char *args[]);
void startPipe(char *line, int ndir, char *dirs[], char *env[]);

char path[512]; // number of dirs

int tokenize(char *line, int *narg, char *args[]) {// YOU have done this in LAB2
                                                   // YOU better know how to apply it from now on
    char *tokens;
    int size = strlen(line) + 1;
    tokens = malloc(size);
    memset(tokens, 0, size);
    char *s;
    strcpy(tokens, line);
    s = strtok(tokens, " ");
    *narg = 0;
    while (s){
        args[(*narg)++] = s; // token string pointers
        s = strtok(0, " ");
    }
    args[*narg] = 0; // arg[n] = NULL pointer
}

void parsePath(char *pathname, char *dir[], int *ndir){
    *ndir = 0;
    dir[0] = strtok(pathname, ":");
    for (int i = 1; dir[i - 1]; i++){
        dir[i] = strtok(NULL, ":");
        (*ndir)++;
        if (*ndir == 64){
            break;
        }
    }
}

void getCommandLine(int ndir, char *dirs[], char *line, char *cmd){
    int is_file = 0;
    for (int i = 0; i < ndir; i++){
        strncpy(line, dirs[i], 128);
        strcat(line, "/");
        strcat(line, cmd);
        fprintf(stderr, "line = %s\n", line);
        if (fopen(line, "r")){
            is_file = 1;
            break;
        }
    }
    if (!is_file){
        fprintf(stderr, "invalid command %s\n", cmd);
        exit(1);
    }
}

void setOutRedir(int nargs, char *args[]){
    for (int i = 0; i < nargs; i++){
        if (!strcmp(args[i], ">")){
            if (i + 1 < nargs){
                char filename[128];
                strncpy(filename, args[i + 1], 128);
                printf("redirecting output to %s\n", filename);
                args[i] = 0;
                int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                break;
            }
        }
        if (!strcmp(args[i], ">>")){
            if (i + 1 < nargs){
                char filename[128];
                strncpy(filename, args[i + 1], 128);
                args[i] = 0;
                close(1);
                open(filename, O_WRONLY | O_APPEND, 0644);
                break;
            }
        }
    }
}

void setInRedir(int nargs, char *args[]){
    for (int i = 0; i < nargs; i++){
        if (!args[i]){
            continue;
        }
        if (!strcmp(args[i], "<")){
            if (i + 1 < nargs){
                char filename[128];
                strncpy(filename, args[i + 1], 128);
                args[i] = 0;
                close(0);
                open(filename, O_RDONLY);
                printf("line: %d\n", __LINE__);
                break;
            }
        }
    }
}

void pipeWriter(char *line_head, int pd[], int ndir, char *dirs[], char *env[]){
    fprintf(stderr, "starting writer process, pid =  %d\n", getpid());
    int nargs;
    char *args[64];
    tokenize(line_head, &nargs, args);
    setInRedir(nargs, args);
    close(pd[0]);
    dup2(pd[1], STDOUT_FILENO);
    close(pd[1]);
    getCommandLine(ndir, dirs, line_head, args[0]);
    int r = execve(line_head, args, env);

    printf("writer: execve failed r = %d\n", r);
    exit(1);
}

void pipeReader(char *line_tail, int pd[], int ndir, char *dirs[], char *env[]){
    fprintf(stderr, "starting reader process, pid =  %d\n", getpid());
    int nargs;
    char *args[64];
    tokenize(line_tail, &nargs, args);
    close(pd[1]);
    dup2(pd[0], STDIN_FILENO);
    close(pd[0]);
    if (isPipe(nargs, args)){
        startPipe(line_tail, ndir, dirs, env);
        exit(0);
    }
    printArgs(nargs, args);
    setOutRedir(nargs, args);
    getCommandLine(ndir, dirs, line_tail, args[0]);
    fprintf(stderr, "line_tail = %s, len = %d\n", line_tail, strlen(line_tail));
    int r = execve(line_tail, args, env);

    printf("reader: execve failed r = %d\n", r);
    exit(1);
}

void printArgs(int nargs, char *args[]){
    for (int i = 0; i < nargs; i++){
        printf("arg[%d] = %s\n", i, args[i]);
    }
}

int isPipe(int nargs, char *args[]){
    for (int i = 0; i < nargs; i++){
        if (!strcmp(args[i], "|")){
            return i;
        }
    }
    return 0;
}

void startPipe(char *line, int ndir, char *dirs[], char *env[]){
    char *line_head = malloc(128);
    char *line_tail = malloc(128);
    int writer_pid, reader_pid;

    strncpy(line_head, line, 128);
    line_head = strtok(line_head, "|");
    line_tail = strtok(NULL, "\n");
    char *child_arg[64];
    int child_narg;
    int status;
    int pd[2];
    pipe(pd);
    writer_pid = fork();
    if (!writer_pid) {// writer
        pipeWriter(line_head, pd, ndir, dirs, env);
    }
    fprintf(stderr, "waiting for writer pid = %d\n", writer_pid);
    waitpid(writer_pid, &status, 0);

    reader_pid = fork();
    if (!reader_pid){
        pipeReader(line_tail, pd, ndir, dirs, env);
    }
}

int main(int argc, char *argv[], char *env[]){
    char *arg[64]; // token string pointers
    int nargs;     // number of token strings
    int pid, status;
    char *cmd;
    char line[128];
    char *dirs[64] = {NULL}; // dir string pointers
    int ndir = 0;

    strncpy(path, getenv("PATH"), 512);
    printf("PATH: %s\n", path);
    parsePath(path, dirs, &ndir);

    printf("ndir = %d\n", ndir);

    // show dirs
    for (int i = 0; i < ndir; i++){
        printf("dirs[%d] = %s\n", i, dirs[i]);
    }

    while (1){
        printf("sh %d running\n", getpid());
        printf("enter a command line : ");
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        if (line[0] == 0)
            continue;

        tokenize(line, &nargs, arg);

        cmd = arg[0]; // line = arg0 arg1 arg2 ...

        if (strcmp(cmd, "cd") == 0){
            chdir(arg[1]);
            continue;
        }
        if (strcmp(cmd, "exit") == 0)
            exit(0);

        pid = fork();

        if (pid){
              printf("sh %d forked a child sh %d\n", getpid(), pid);
              printf("sh %d wait for child sh %d to terminate\n", getpid(), pid);
              waitpid(pid, status, 0);
              printf("ZOMBIE child=%d exitStatus=%x\n", pid, status);
              printf("main sh %d repeat loop\n", getpid());
        }
        else{
              printf("child sh %d running\n", getpid());

              printArgs(nargs, arg);
              if (isPipe(nargs, arg)){
                startPipe(line, ndir, dirs, env);
                fprintf(stderr, "child process done, pid = %d\nWait 5 seconds...\n", getpid());
                exit(0);
              }  

              getCommandLine(ndir, dirs, line, cmd);
              setOutRedir(nargs, arg);
              setInRedir(nargs, arg);
              int r = execve(line, arg, env);

              printf("execve failed r = %d\n", r);
              exit(1);
          }
      }
}