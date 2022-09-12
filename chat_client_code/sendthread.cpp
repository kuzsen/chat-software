#include "sendthread.h"

SendThread::SendThread(QJsonObject obj)
{
    port = obj.value("port").toInt();
    fileName = obj.value("filename").toString();
    fileLength = obj.value("length").toInt();
}

void SendThread::run()
{
    QTcpSocket sendSocket;
    sendSocket.connectToHost(QHostAddress("47.101.128.140"), port);
    if (!sendSocket.waitForConnected(10000))
    {
        this->quit();
    }
    else
    {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);

        while (1)
        {
            QByteArray ba = file.read(1024 * 1024);
            if (ba.size() == 0)
            {
                break;
            }
            sendSocket.write(ba);
            sendSocket.flush();
            usleep(2000000);
        }
        file.close();
        sendSocket.close();
        this->quit();
    }
}
