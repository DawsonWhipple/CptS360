#ifndef REMOTE_H
#define REMOTE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "client.h"

int remote_rm(char* line);
int remote_mkdir(char* line);
int remote_rmdir(char* line);
int remote_pwd(char* line);
int remote_cd(char* line);
int remote_ls(char* line);
int remote_put(char* line);
int remote_get(char* line);

#endif