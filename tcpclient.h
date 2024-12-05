#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <WinSock2.h>
class TcpClient
{
public:
    TcpClient();
    ~TcpClient();

    void Initial();

    void Com(SOCKET fd);

    bool stop;

//    SOCKET fd;
};

#endif // TCPCLIENT_H
