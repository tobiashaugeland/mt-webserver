#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    if (argc > 2){
    char  wwwpath[256];
    strcpy(wwwpath, argv[1]);
    int port = atoi(argv[2]);

    printf("Path: %s\nPort: %d\n",wwwpath,port);

    }
    return 0;
}
