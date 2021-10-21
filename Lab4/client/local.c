#include "local.h"

int local_rm(char *line)
{
    char line_copy[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    r = unlink(path);
    if (-1 == r)
    {
        printf("failed to remove file: %s\n", path);
    }
    return r;
}

int local_mkdir(char *line)
{
    char line_copy[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    r = mkdir(path, 0755);
    if (-1 == r)
    {
        printf("failed to make directory: %s\n", path);
    }
    return r;
}

int local_rmdir(char *line)
{
    char line_copy[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    r = rmdir(path);
    if (-1 == r)
    {
        printf("failed to remove directory: %s\n", path);
    }
    return r;
}

int local_pwd(char *line)
{
    char buf[MAX];
    getcwd(buf, MAX);

    printf("pwd: %s\n", buf);

    return 0;
}

int local_cd(char *line)
{
    char line_copy[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    int r = chdir(path);
    if (-1 == r)
    {
        printf("failed to change to directory: %s\n", path);
    }
    return r;
}

int local_ls(char *line)
{
    DIR *directory;
    struct dirent *contents;
    struct stat fileStat;
    struct passwd *pw;
    struct group *gr;
    struct tm *timeinfo;
    int r;
    char dir_name[MAX];
    char full_path[MAX * 2];
    char buf[1024];
    int files = 0;
    if (strcmp(line, "lls"))
    {
        char line_copy[MAX];
        strcpy(line_copy, line);
        strtok(line_copy, " ");
        strcpy(dir_name, strtok(0, " "));
    }
    else
    {
        strcpy(dir_name, ".");
    }
    printf("dirname = %s\n", dir_name);
    directory = opendir(dir_name);
    if (!directory)
    {
        printf("failed to read directory %s, errno = %d\n", dir_name, errno);
        printf("error: %s\n", strerror(errno));
        return -1;
    }
    while ((contents = readdir(directory)))
    {
        sprintf(full_path, "%s/%s", dir_name, contents->d_name);
        r = stat(full_path, &fileStat);
        if (r)
        {
            printf("error: %s\n", strerror(errno));
            continue;
        }
        pw = getpwuid(fileStat.st_uid);
        gr = getgrgid(fileStat.st_gid);
        timeinfo = localtime(&fileStat.st_ctime);
        sprintf(buf, (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IRUSR) ? "r" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IWUSR) ? "w" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IXUSR) ? "x" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IRGRP) ? "r" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IWGRP) ? "w" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IXGRP) ? "x" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IROTH) ? "r" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IWOTH) ? "w" : "-");
        sprintf(buf + strlen(buf), (fileStat.st_mode & S_IXOTH) ? "x" : "-");
        sprintf(buf + strlen(buf), " %3d", fileStat.st_nlink);
        sprintf(buf + strlen(buf), " %s", (pw) ? pw->pw_name : "---");
        sprintf(buf + strlen(buf), " %s", (gr) ? gr->gr_name : "---");
        sprintf(buf + strlen(buf), " %5d", fileStat.st_size);
        sprintf(buf + strlen(buf), " %s", asctime(timeinfo));
        sprintf(buf + strlen(buf) - 1, " %s", contents->d_name);
        printf("%s\n", buf);
    }
    closedir(directory);
    return 0;
}

int local_cat(char *line)
{
    char buf[4097];
    char line_copy[MAX];
    strcpy(line_copy, line);
    char *source;
    strtok(line_copy, " ");
    source = strtok(0, " ");
    printf("source = %s\n", source);

    int source_fd = open(source, O_RDONLY);
    if (-1 == source_fd)
    {
        printf("failed to open source file %s, errno = %d\n", source, errno);
        printf("error: %s\n", strerror(errno));
        return -1;
    }
    ssize_t byte_size;
    while (byte_size = read(source_fd, buf, 4096))
    {
        if (-1 == byte_size)
        {
            printf("failed to read bytes\n");
            return -1;
        }
        buf[byte_size] = 0;
        printf("%s", buf);
    }
    printf("\n");
    close(source);
    return 0;
}
