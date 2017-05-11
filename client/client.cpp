#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if (argc!=3) {
        printf("usage: %s [IP] [PORT]\n", argv[0]);
        printf("ex) %s 127.0.0.1 1500\n", argv[0]);
        exit(0);
    } else {
        printf("IP: %s, PORT: %s\n", argv[1], argv[2]);
    }

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        printf("Client: Can't open stream socket!\n");
        exit(0);
    }

    struct sockaddr_in server_addr;
    //unsigned int ipaddr = inet_addr(argv[1]);
    memset(&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //server_addr.sin_addr.s_addr = ipaddr;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))
            == -1) {
        printf("Client: Can't connect to %s:%s!\n", argv[1], argv[2]);
        exit(0);
    }

    while(true) {
        char message[30];
        int len = read(client_fd, message, sizeof(message)-1);
        if (len == -1) {
            printf("Client: Read error!\n");
            exit(0);
        }
        message[len] = '\0';
        printf("Message from server: %s\n", message);
    }
    close(client_fd);

    return 0;
}
