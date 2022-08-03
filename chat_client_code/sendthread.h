#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QFile>

class SendThread : public QThread
{
public:
    SendThread(QJsonObject obj);
    void run();

private:
    QString fileName;
    int fileLength;
    int port;
};

#endif // SENDTHREAD_H
