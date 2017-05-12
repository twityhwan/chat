#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

static void* readTask(void* data) {
    char message[1024];
    int len = 0;
    int client_fd = *((int*)data);

    while( true) {
        len = read(client_fd, message, sizeof(message));
        if(len > 0) printf("> %s\n", message);
        else if(len <= 0) exit(1);
    }
}

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
    memset(&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))
            == -1) {
        printf("Client: Can't connect to %s:%s!\n", argv[1], argv[2]);
        exit(0);
    }

    char message[1024];
    int len = read(client_fd, message, sizeof(message)-1);
    if (len < 1) {
        printf("Client: Read error!\n");
        exit(0);
    }

    printf("Message from server: %s\n", message);

    pthread_t t;
    pthread_create(&t, NULL, readTask, &client_fd);

    while(true) {
        fgets(message, sizeof(message), stdin);
        message[strlen(message)-1] = '\0';
        if (strcmp(message, "quit")==0) {
            printf("Client closed\n");
            break;
        }
        write(client_fd, message, strlen(message)+1);
    }
    close(client_fd);

    return 0;
}
