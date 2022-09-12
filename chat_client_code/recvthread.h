#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>

class RecvThread : public QThread
{
    Q_OBJECT
public:
    RecvThread(QJsonObject obj);
    void run();

private slots:
    void recv_file();

private:
    QString fileName;
    int fileLength;
    int total;
    int port;
    QTcpSocket *recvSocket;
    QFile *file;
};

#endif // RECVTHREAD_H
