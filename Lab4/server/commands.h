#ifndef COMMANDS_H
#define COMMANDS_H

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include "server.h"

int server_rm(char* line);
int server_mkdir(char* line);
int server_rmdir(char* line);
int server_pwd(char* line);
int server_cd(char* line);
int server_ls(char* line);
int server_put(char* line);
int server_get(char* line);

#endif