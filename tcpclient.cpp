#include "tcpclient.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>//主要提供对POSIX操作系统接口的访问，使用io.h进行替代
#include <io.h>
//#include <arpa/inet.h>//使用winsock2.h

#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>//inet_ntop
unsigned short PROT = 9999;

#include <QtConcurrent>
TcpClient::TcpClient()
    :stop(false)
{

}

TcpClient::~TcpClient()
{
    stop = true;
    //6.关闭文件描述符
//    closesocket(fd);
    //_close(fd);
    WSACleanup();
}

void TcpClient::Initial()
{
    //windows需要初始化socket
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    //1.创建通信的套接字
    SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return;
    }
    //2.连接服务器IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PROT);//大端
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.S_un.S_addr );
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("connect");
        return;
    }
    //此处若全部使用引用捕获，可能会导致传入Com的值是错误的，因为fd在Initial中是局部变量！！！
    QtConcurrent::run([this,fd](){
        this->Com(fd);
    });
}

void TcpClient::Com(SOCKET fd)
{
    //3.通信
    int number = 0;
    while (!stop) {
        //发送数据
        char buff[1024];
        sprintf_s(buff, "from client : hello , %d", number++);
        send(fd, buff, sizeof(buff), 0);
        Sleep(1000);
        memset(buff, 0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        if (len > 0) {
            printf("from server : %s \n", buff);
        }
        else if (len == 0) {
            printf("服务端已经断开连接");
            break;
        }
        else {
            if (errno == EINTR) {
                // 信号中断，可以选择忽略或重新开始recv
                break;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 非阻塞模式下没有数据，可以稍后再尝试接收
                break;
            }
            else if (errno == ECONNRESET || errno == EPIPE) {
                // 连接被对方重置或对端已关闭
                break;
            }
            else {
                // 其他错误，根据具体错误处理
                perror("recv failed");
                break;
            }
        }
        Sleep(1000);
    }
    closesocket(fd);
}
