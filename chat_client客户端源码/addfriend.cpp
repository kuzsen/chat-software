#include "addfriend.h"
#include "ui_addfriend.h"

Addfriend::Addfriend(QTcpSocket *s, QString u, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Addfriend)
{
    ui->setupUi(this);
    this->socket = s;
    userName = u;
}

Addfriend::~Addfriend()
{
    delete ui;
}

void Addfriend::on_cancelButton_clicked()
{
    this->close();
}

void Addfriend::on_addButton_clicked()
{
    QString friendName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "add");
    obj.insert("user", userName);
    obj.insert("friend", friendName);

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    this->close();
}
