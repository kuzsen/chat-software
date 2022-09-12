#include "addgroup.h"
#include "ui_addgroup.h"

AddGroup::AddGroup(QTcpSocket *s, QString u, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddGroup)
{
    ui->setupUi(this);
    socket = s;
    userName = u;
}

AddGroup::~AddGroup()
{
    delete ui;
}

void AddGroup::on_cancelButton_clicked()
{
    this->close();
}

void AddGroup::on_addButton_clicked()
{
    QString groupName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "add_group");
    obj.insert("user", userName);
    obj.insert("group", groupName);
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
    this->close();
}
