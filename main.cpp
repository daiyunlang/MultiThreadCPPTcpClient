#include <QCoreApplication>
#include "tcpclient.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TcpClient tcl;
    tcl.Initial();
    return a.exec();
}
