#include "commands.h"

int server_rm(char *line)
{
    printf("%s\n", __FUNCTION__);
    char line_copy[MAX];
    strcpy(line_copy, line);
    char *path;
    char buf[MAX];
    strtok(line_copy, " ");
    path = strtok(0, " ");
    r = unlink(path);
    if (-1 == r)
    {
        sprintf(buf, "Failed to remove file: %s\n", path);
        printf("%s", buf);
        n = write(csock, buf, MAX);
        return 0;
    }
    sprintf(buf, "succesfully removed file: %s\n", path);
    printf("%s", buf);
    n = write(csock, buf, MAX);
    return r;
}

int server_mkdir(char *line)
{
    printf("%s\n", __FUNCTION__);
    char line_copy[MAX];
    char buf[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    r = mkdir(path, 0755);
    if (-1 == r)
    {
        sprintf(buf, "Failed to make directory: %s\n", path);
        printf("%s", buf);
        n = write(csock, buf, MAX);
        return 0;
    }
    sprintf(buf, "succesfully made directory: %s\n", path);
    printf("%s", buf);
    n = write(csock, buf, MAX);
    return r;
}

int server_rmdir(char *line)
{
    printf("%s\n", __FUNCTION__);
    char line_copy[MAX];
    char buf[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    r = rmdir(path);
    if (-1 == r)
    {
        sprintf(buf, "Failed to remove directory: %s\n", path);
        printf("%s", buf);
        n = write(csock, buf, MAX);
        return 0;
    }
    sprintf(buf, "succesfully removed directory: %s\n", path);
    printf("%s", buf);
    n = write(csock, buf, MAX);
    return r;
}

int server_pwd(char *line)
{
    printf("%s\n", __FUNCTION__);
    char buf[MAX];
    getcwd(buf, MAX);

    write(csock, buf, MAX);

    return 0;
}

int server_cd(char *line)
{
    printf("%s\n", __FUNCTION__);
    char line_copy[MAX];
    char buf[MAX];
    strcpy(line_copy, line);
    char *path;
    strtok(line_copy, " ");
    path = strtok(0, " ");
    int r = chdir(path);
    if (-1 == r)
    {
        sprintf(buf, "Failed to change to directory: %s\n", path);
        printf("%s", buf);
        n = write(csock, buf, MAX);
        return 0;
    }
    sprintf(buf, "succesfully changed to directory: %s\n", path);
    printf("%s", buf);
    n = write(csock, buf, MAX);
    return r;
}

int server_ls(char *line)
{
    printf("%s\n", __FUNCTION__);
    DIR *directory;
    struct dirent *contents;
    struct stat fileStat;
    struct passwd *pw;
    struct group *gr;
    struct tm * timeinfo;
    char dir_name[MAX];
    char full_path[MAX * 2];
    char buf[1024];
    int files = 0;
    if (strcmp(line, "ls"))
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
        strcpy(buf, "end");
        n = write(csock, buf, MAX);
        printf("error: %s\n", strerror(errno));
        return 0;
    }
    while ((contents = readdir(directory)))
    {
        files++;
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
        printf("%s", buf);
        n = write(csock, buf, 1024);
    }
    sprintf(buf, "end");
    printf("%s", buf);
    n = write(csock, buf, MAX);
    closedir(directory);
    return 0;
}

int server_put(char *line)
{
    printf("%s\n", __FUNCTION__);
    char line_copy[MAX];
    char buf[MAX];
    char *size_string;
    char *path;
    FILE *file;

    memset(buf, 0, MAX);
    strcpy(line_copy, line);
    strtok(line_copy, " ");
    size_string = strtok(0, " ");
    path = strtok(0, " ");
    int size = atoi(size_string);
    int bytes_written = 0;
    printf("path: %s\n", path);

    file = fopen(path, "w");

    if (!file)
    {
        sprintf(buf, "could not open %s for writing\n", path);
        printf("%s", buf);
        n = write(csock, buf, MAX);
        return 0;
    }
    sprintf(buf, "ok");
    printf("%s", buf);
    n = write(csock, buf, MAX);
    while (1)
    {
        n = read(csock, buf, MAX);
        if (n == 0)
        {
            printf("server: client died, server loops\n");
            return -1;
        }
        if ((bytes_written + n) > size)
        {
            n = size - bytes_written;
        }
        printf("\nrecieved: n=%d, buf= %s\n", n, buf);
        write(fileno(file), buf, n);
        bytes_written += n;
        if (bytes_written == size)
        {
            break;
        }
    }
    fclose(file);
    return 0;
}

int server_get(char *line)
{
    printf("%s\n", __FUNCTION__);
    printf("function: %s\n", __FUNCTION__);
    int is_file = 0;
    char *path;
    char line_copy[MAX];
    char buf[MAX];

    strcpy(line_copy, line);
    //parse command
    strtok(line_copy, " ");
    path = strtok(0, " ");
    //check file exists and is readable
    //if okay to proceed send file
    int source_fd = open(path, O_RDONLY);
    if (-1 == source_fd)
    {
        printf("failed to open source file %s, errno = %d\n", path, errno);
        printf("error: %s\n", strerror(errno));
        sprintf(buf, "-1");
        write(csock, buf, MAX);
        return 0;
    }
    //get file size
    struct stat st;
    stat(path, &st);
    printf("file: %s; size is %ld bytes\n", path, st.st_size);

    sprintf(buf, "%ld", st.st_size);
    n = write(csock, buf, MAX);
    printf("server: wrote n=%d bytes; line=%s\n", n, buf);

    ssize_t byte_size;
    int byte_sent = 0;
    while (1)
    {
        memset(buf, 0, MAX);
        byte_size = read(source_fd, buf, MAX);
        if (!byte_size)
        {
            break;
        }
        if (-1 == byte_size)
        {
            printf("failed to read bytes\n");
            return -1;
        }
        printf("sending: %s\n", buf);
        n = write(csock, buf, MAX);
        byte_sent += byte_size;
        if (byte_sent > st.st_size)
        {
            printf("read more than file size, byte sent = %d bytes\n", byte_sent);
        }
        printf("server: sent %d bytes\n", n);
    }
    close(path);

    return 0;
}
