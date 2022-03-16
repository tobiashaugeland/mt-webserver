#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "bbuffer.h"

#define DEFAULT_PORT 8000
#define DEFAULT_ROOT_PATH "webroot"

char wwwpath[256] = DEFAULT_ROOT_PATH;

void *handle_request(void *bb)
{
    while (1)
    {
        sleep(1);
        int fd = bb_get(bb);
        char buffer[1024];
        read(fd, buffer, sizeof(buffer));
        char req_type[256];
        char req_path[256];
        int seq;
        sscanf(buffer, "%s %s %d", req_type, req_path, seq);
        printf("Type: %s Path: %s Seq:%d\n", req_type, req_path, seq);
        char full_path[256];
        strcpy(full_path, wwwpath);
        strcat(full_path, req_path);

        FILE *fp;
        if (access(full_path, F_OK) == 0)
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
            send(fd, error_msg, sizeof(error_msg), 0);
        }
        close(fd);
    }
}

int main(int argc, char const *argv[])
{
    int socket_fd, new_socket_fd;
    int opt = 1;
    int port = DEFAULT_PORT;
    char buffer[1024];
    struct sockaddr_in6 address;
    int addrlen = sizeof(address);

    if (argc > 2)
    {
        strcpy(wwwpath, argv[1]);
        port = atoi(argv[2]);
    }
    printf("Path: %s\nPort: %d\n", wwwpath, port);

    address.sin6_family = AF_INET6;
    address.sin6_port = htons(port);
    address.sin6_addr = in6addr_any;

    // Allows both ipv4 and ipv6 connections
    // To test with ipv4: curl --http0.9 127.0.0.1:8000/index.html
    // To test with ipv6: curl --http0.9 -g -6 "[::1]:8000/index.html"
    socket_fd = socket(AF_INET6, SOCK_STREAM, 0);

    // Set options to force socket to bind, even if address or port is already in use
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(socket_fd, (struct sockaddr *)&address, sizeof(address));

    listen(socket_fd, 1);

    BNDBUF *bb = bb_init(100);
    pthread_t threads[4];
    for (int i = 0; i < 4; i++)
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
