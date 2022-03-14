#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int port, socket_fd, new_socket_fd;
    int opt = 1;
    char wwwpath[256], buffer[1024];
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if (argc > 2)
    {
        strcpy(wwwpath, argv[1]);
        port = atoi(argv[2]);

        printf("Path: %s\nPort: %d\n", wwwpath, port);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Set options to force socket to bind, even if address or port is already in use
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    bind(socket_fd, (struct sockaddr *)&address, sizeof(address));

    listen(socket_fd, 1);

    printf("Ready to accept connections...\n");

    while (1)
    {
        new_socket_fd = accept(socket_fd, (struct sockaddr *)&address,
                               (socklen_t *)&addrlen);

        read(new_socket_fd, buffer, sizeof(buffer));
        char req_type[256];
        char req_path[256];
        sscanf(buffer, "%s %s", req_type, req_path);
        printf("Type: %s\nPath: %s\n", req_type, req_path);
        char full_path[256];
        strcpy(full_path, wwwpath);
        strcat(full_path, req_path);
        printf("%s\n", full_path);

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
            int sent_bytes = send(new_socket_fd, file_buf, sizeof(file_buf), 0);
        }
        else
        {
            char error_msg[] = "Error 404: File not found\n";
            send(new_socket_fd, error_msg, sizeof(error_msg), 0);
        }
        close(new_socket_fd);
    }
    return 0;
}
