#ifndef _CHAT_SERVER_H_
#define _CHAT_SERVER_H_

#include <netinet/in.h>

#define BUF_LEN 128

class ChatServer
{
public:
    ChatServer();
    ~ChatServer();
    void start();
    bool isConnected() { return m_isConnected; }
private:
    bool m_isConnected;
    int m_port;
    int m_fd;
    //std::vector<SOCKET> m_clients;
    struct sockaddr_in m_addr;
};

#endif // _CHAT_SERVER_H_
