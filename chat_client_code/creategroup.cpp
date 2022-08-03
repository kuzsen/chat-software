#include "creategroup.h"
#include "ui_creategroup.h"

CreateGroup::CreateGroup(QTcpSocket *s, QString u, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateGroup)
{
    ui->setupUi(this);
    socket = s;
    userName = u;
}

CreateGroup::~CreateGroup()
{
    delete ui;
}

void CreateGroup::on_cancelButton_clicked()
{
    this->close();
}

void CreateGroup::on_pushButton_2_clicked()
{
    QString groupName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "create_group");
    obj.insert("user", userName);
    obj.insert("group", groupName);
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
    this->close();
}
