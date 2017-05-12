#include "ChatServer.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

using namespace std;

#define BUF_LEN 128

static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;

namespace {
struct Client {
    struct sockaddr_in from;
    int fd;
};
}

static vector<Client> s_clients;

static void *chatTask(void *);
static bool closeClient(int fd) {
    printf("close client (%d)\n", fd);
    pthread_mutex_lock(&s_lock);
    for(vector<Client>::iterator it = s_clients.begin(); it != s_clients.end(); it++) {
        if(it->fd == fd) {
            close(fd);
            s_clients.erase(it);
            pthread_mutex_unlock(&s_lock);
            return true;
        }
    }
    pthread_mutex_unlock(&s_lock);
    return false;
}

static void broadCast(int fd, const char* buf, int len) {
    pthread_mutex_lock(&s_lock);
    for(vector<Client>::iterator it = s_clients.begin();
            it != s_clients.end(); it++) {
        if (it->fd == fd) continue;
        write(it->fd, buf, len);
    }
    pthread_mutex_unlock(&s_lock);
}

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
        int use = 1;
        setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&use, sizeof(int));
        if (m_fd == -1) {
            cout<<"Server: Can't open stream socket!"<<endl;
            exit(0);
        }
    }

    if (bind(m_fd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0) {
        cout<<"Server: Can't bind local address!"<<endl;
        exit(0);
    }

    if (listen(m_fd, 10) < 0) {
        cout<<"Server: Can't listen connection!"<<endl;
        exit(0);
    }

    cout<<"Server("<<m_port<<") : waiting connection request.."<<endl;

    struct sockaddr_in client_addr;
    char buffer[BUF_LEN];
    socklen_t len = sizeof(client_addr);
    char temp[20];

    while(true) {
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

        Client c;
        c.fd = client_fd;
        c.from = client_addr;
        s_clients.push_back(c);
        pthread_t t;
        pthread_create(&t, NULL, chatTask, (void*)&(s_clients.back()));
    }
}

void *chatTask(void *data) {
    char buffer[BUF_LEN];
    Client* c = reinterpret_cast<Client*>(data);
    int client_fd = c->fd;
    while(1) {
        memset(buffer, 0x00, BUF_LEN);
        int msg_size = read(client_fd, buffer, BUF_LEN);
        if (msg_size < 0) {
            break;
        }
        if (msg_size == 0) continue;
        printf("message received from c(%d), msg:%s\n", client_fd, buffer);
        broadCast(client_fd, buffer, msg_size);
    }
    closeClient(client_fd);
}
