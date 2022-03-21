#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bbuffer.h"

#define DEFAULT_PORT 8000
#define DEFAULT_ROOT_PATH "webroot"
#define DEFAULT_THREAD_COUNT 4
#define DEFAULT_BUFFER_SIZE 10

char wwwpath[256] = DEFAULT_ROOT_PATH;

/**
 * @brief Checks if the given path is a directory that is specified in the
 * wwwpath variable.
 *
 * @param path The path to check.
 * @return 1 if the path is a directory in the wwwpath, 0 otherwise.
 */
int good_request(char path[])
{
    if (strstr(path, "..") != NULL)
    {
        return 0;
    }

    char working_dir[256];
    char resolved_path[256];
    realpath(path, resolved_path);
    realpath(wwwpath, working_dir);
    if (strstr(resolved_path, working_dir) == NULL)
    {
        return 0;
    }
    return 1;
}

/**
 * @brief Checks if the given path is a file, not a directory.
 *
 * @param path The path to check.
 * @return int 1 if the path is a file, 0 otherwise.
 */
int is_regular_file(char path[])
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/**
 * @brief consumer thread handler
 *
 * @param bb ring buffer
 */
void *handle_request(void *bb)
{
    while (1)
    {
        int fd = bb_get(bb);
        char buffer[1024];
        read(fd, buffer, sizeof(buffer));
        char req_type[256];
        char req_path[256];
        sscanf(buffer, "%s %s", req_type, req_path);
        char full_path[256];
        strcpy(full_path, wwwpath);
        strcat(full_path, req_path);

        FILE *fp;
        if ((access(full_path, F_OK) == 0) && good_request(full_path) && is_regular_file(full_path))
        {
            fp = fopen(full_path, "r");
            fseek(fp, 0, SEEK_END);
            size_t file_size = ftell(fp);
            rewind(fp);
            char file_buf[file_size];
            memset(file_buf, 0, sizeof(file_buf));
            fread(file_buf, sizeof(file_buf), 1, fp);
            fclose(fp);
            int sent_bytes = send(fd, file_buf, sizeof(file_buf), 0);
        }

        else
        {
            char error_msg[] = "Error 404: File not found\n";
            send(fd, error_msg, strlen(error_msg), 0);
        }
        close(fd);
    }
}

/***************************************************
 *                      MAIN                       *
 **************************************************/
int main(int argc, char const *argv[])
{
    int socket_fd, new_socket_fd;
    int opt = 1;
    int port = DEFAULT_PORT;
    int thread_count = DEFAULT_THREAD_COUNT;
    int buffer_size = DEFAULT_BUFFER_SIZE;
    char buffer[1024];
    struct sockaddr_in6 address;
    int addrlen = sizeof(address);

    // Argument parsing, if 4 arguments are not given it will use defaults
    if (argc == 5)
    {
        strcpy(wwwpath, argv[1]);
        port = atoi(argv[2]);
        thread_count = atoi(argv[3]);
        buffer_size = atoi(argv[4]);
    }
    else
    {
        printf("Correct arguments were not given, using defaults\n"
               "Correct usage is: mtwwwd <www-path> <port> <threads> <bufferslots>\n");
    }
    printf("Path: %s\nPort: %d\nThread count: %d\nBuffer slots: %d\n", wwwpath, port, thread_count, buffer_size);

    address.sin6_family = AF_INET6;
    address.sin6_port = htons(port);
    address.sin6_addr = in6addr_any;

    // Allows both ipv4 and ipv6 connections
    // To test with ipv4: curl --http0.9 127.0.0.1:8000/index.html
    // To test with ipv6: curl --http0.9 -g -6 "[::1]:8000/index.html"
    if ((socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    // Set options to force socket to bind, even if address or port is already in use
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Failed to set socket options");
        exit(1);
    }

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Failed to bind socket");
        exit(1);
    }

    listen(socket_fd, 1);

    BNDBUF *bb = bb_init(buffer_size);

    if (bb == NULL)
    {
        perror("Failed to initialize buffer");
        exit(1);
    }

    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, &handle_request, bb);
    }

    printf("Ready to accept connections...\n");
    while (1)
    {
        new_socket_fd = accept(socket_fd, (struct sockaddr *)&address,
                               (socklen_t *)&addrlen);
        bb_add(bb, new_socket_fd);
    }
    return 0;
}
