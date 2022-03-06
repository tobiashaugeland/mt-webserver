#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int port, socket_fd, new_socket_fd, valread;
    int opt = 1;
    char wwwpath[256], buffer[1024];
    char reply[] = "Hello man";
    struct sockaddr_in address;

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

    while (1)
    {
        new_socket_fd = accept(socket_fd, (struct sockaddr *)&address,
                               (socklen_t *)&address);

        read(new_socket_fd, buffer, sizeof(buffer));
        printf("Data received: %s\n", buffer);
        send(new_socket_fd, buffer, strlen(buffer), 0);
        close(new_socket_fd);
    }
    return 0;
}
