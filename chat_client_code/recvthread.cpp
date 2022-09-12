#include "recvthread.h"

RecvThread::RecvThread(QJsonObject obj)
{
    total = 0;
    port = obj.value("port").toInt();
    fileLength = obj.value("length").toInt();

    QString pathName = obj.value("filename").toString();
    QStringList strList = pathName.split('/');
    fileName = strList.at(strList.size() - 1);
}

void RecvThread::run()
{
    file = new QFile(fileName);
    file->open(QIODevice::WriteOnly);

    recvSocket = new QTcpSocket;

    connect(recvSocket, &QTcpSocket::readyRead, this, &RecvThread::recv_file, Qt::DirectConnection);

    recvSocket->connectToHost(QHostAddress("47.101.128.140"), port);
    if (!recvSocket->waitForConnected(10000))
    {
        this->quit();
    }
    else
    {

    }

    exec();
}

void RecvThread::recv_file()
{
    QByteArray ba = recvSocket->readAll();
    total += ba.size();
    file->write(ba);

    if (total >= fileLength)
    {
        file->close();
        recvSocket->close();
        delete file;
        delete recvSocket;
        this->quit();
    }
}
