#include "ChatServer.h"
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

#define BUF_LEN 128
#define MAX_CLIENT 5

int threadNum = 0;
pthread_t threadArr[MAX_CLIENT];

void *chatTask(void *);

ChatServer::ChatServer() : m_isConnected(false), m_port(1500),
    m_fd(-1)
{
    memset(&m_addr, 0x00, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = htons(INADDR_ANY);
    m_addr.sin_port = htons(m_port);
}

ChatServer::~ChatServer()
{
    close(m_fd);
}

void
ChatServer::start()
{
    if (m_fd == -1) {
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_fd == -1) {
            cout<<"Server: Can't open stream socket!"<<endl;
            exit(0);
        }
    }

    if (bind(m_fd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0) {
        cout<<"Server: Can't bind local address!"<<endl;
        exit(0);
    }

    if (listen(m_fd, MAX_CLIENT) < 0) {
        cout<<"Server: Can't listen connection!"<<endl;
        exit(0);
    }

    cout<<"Server("<<m_port<<") : waiting connection request.."<<endl;

    struct sockaddr_in client_addr;
    char buffer[BUF_LEN];
    socklen_t len = sizeof(client_addr);
    char temp[20];

    while(threadNum < MAX_CLIENT) {
        int client_fd = accept(m_fd, (struct sockaddr *)&client_addr, &len);
        if (client_fd < 0) {
            cout<<"Server: accept failed!"<<endl;
            exit(0);
        }

        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
        cout<<"Server: "<<temp<<" client connected."<<endl;
        memset(buffer, 0x00, BUF_LEN);
        strcpy(buffer, "Server connected...\n");
        send(client_fd, buffer, BUF_LEN, 0);

        pthread_create(&threadArr[threadNum], NULL, chatTask, (void*)(intptr_t)client_fd);
        threadNum++;
    }
}

void *chatTask(void *data) {
    char buffer[BUF_LEN];
    int client_fd = (intptr_t)data;
    while(1) {
        memset(buffer, 0x00, BUF_LEN);
        int msg_size = read(client_fd, buffer, BUF_LEN);
        write(client_fd, buffer, msg_size);
        cout<<buffer<<endl;
    }
}
