#include "remote.h"

static char ans[MAX];
static int n;

int remote_rm(char *line)
{
    printf("function: %s\n", __FUNCTION__);
    // Send line to server
    n = write(sock, line, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, line);
    // Read a line from sock and show it
    n = read(sock, ans, MAX);
    printf("client: read n=%d bytes; echo=%s\n", n, ans);

    return 0;
}

int remote_mkdir(char *line)
{
    printf("function: %s\n", __FUNCTION__);
    // Send line to server
    n = write(sock, line, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, line);
    // Read a line from sock and show it
    n = read(sock, ans, MAX);
    printf("client: read n=%d bytes; echo=%s\n", n, ans);

    return 0;
}

int remote_rmdir(char *line)
{
    printf("function: %s\n", __FUNCTION__);
    // Send line to server
    n = write(sock, line, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, line);
    // Read a line from sock and show it
    n = read(sock, ans, MAX);
    printf("client: read n=%d bytes; echo=%s\n", n, ans);

    return 0;
}

int remote_pwd(char *line)
{
    printf("function: %s\n", __FUNCTION__);
    // Send line to server
    n = write(sock, line, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, line);
    // Read a line from sock and show it
    n = read(sock, ans, MAX);
    printf("client: read n=%d bytes; echo=%s\n", n, ans);

    return 0;
}

int remote_cd(char *line)
{
    printf("function: %s\n", __FUNCTION__);
    // Send line to server
    n = write(sock, line, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, line);
    // Read a line from sock and show it
    n = read(sock, ans, MAX);
    printf("client: read n=%d bytes; echo=%s\n", n, ans);

    return 0;
}

int remote_ls(char *line)
{
    char answer[1024];
    printf("function: %s\n", __FUNCTION__);
    // Send line to server
    n = write(sock, line, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, line);
    n = read(sock, answer, 1024);
    printf("client: read n=%d bytes; echo=%s\n", n, answer);
    while (1)
    {
        if (!strcmp(answer, "end"))
        {
            break;
        }
        printf("%s\n", answer);
        n = read(sock, answer, 1024);
    }

    return 0;
}

int remote_put(char *line)
{
    printf("function: %s\n", __FUNCTION__);
    int is_file = 0;
    char *lpath;
    char *rpath;
    char line_copy[MAX];
    strcpy(line_copy, line);
    //parse command
    strtok(line_copy, " ");
    lpath = strtok(0, " ");
    rpath = strtok(0, " ");
    //check file exists and is readable
    //if okay to proceed send file
    int source_fd = open(lpath, O_RDONLY);
    if (-1 == source_fd)
    {
        printf("failed to open source file %s, errno = %d\n", lpath, errno);
        printf("error: %s\n", strerror(errno));
        return -1;
    }
    //get local file size
    struct stat st;
    stat(lpath, &st);
    printf("local file size is %ld bytes\n", st.st_size);
    char buf[MAX];

    sprintf(buf, "put %ld %s", st.st_size, rpath);
    //send a message to the server checking if the location to be written is writable
    n = write(sock, buf, MAX);
    printf("client: wrote n=%d bytes; line=%s\n", n, buf);

    //wait for response from server if ok to proceed

    n = read(sock, ans, MAX);
    printf("client: read n=%d bytes; line=%s\n", n, ans);

    //error message return -1
    if (strcmp(ans, "ok"))
    {
        printf("could not upload file, server response: %s\n", ans);
        return -1;
    }
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
        n = write(sock, buf, MAX);
        byte_sent += byte_size;
        if (byte_sent > st.st_size)
        {
            printf("read more than file size, byte sent = %d bytes\n", byte_sent);
        }
        printf("client: sent %d bytes\n", n);
    }
    close(lpath);

    return 0;
}

int remote_get(char *line)
{
    printf("%s\n", __FUNCTION__);
    char line_copy[MAX];
    char buf[MAX];
    char *lpath;
    char *rpath;
    int size;
    int bytes_written = 0;
    FILE *file;

    memset(buf, 0, MAX);
    strcpy(line_copy, line);
    strtok(line_copy, " "); // remove "get" command
    rpath = strtok(0, " ");
    lpath = strtok(0, " ");

    printf("path: %s\n", lpath);

    file = fopen(lpath, "w");

    if (!file)
    {
        printf("could not open %s for writing\n", lpath);
        return 0;
    }
    sprintf(buf, "get %s", rpath);
    printf("%s", buf);
    n = write(sock, buf, MAX);

    n = read(sock, ans, MAX);

    size = atoi(ans);

    if(size < 0)
    {
        printf("get failed, server error\n");
        return 0;
    }
    while (1)
    {
        n = read(sock, buf, MAX);
        if (n == 0)
        {
            printf("server died\n");
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
