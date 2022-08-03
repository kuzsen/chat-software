#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    socket = new QTcpSocket;
    socket->connectToHost(QHostAddress("47.101.128.140"), 8000);

    connect(socket, &QTcpSocket::connected, this, &Widget::connect_success);
    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::connect_success()
{
    QMessageBox::information(this, "连接提示", "连接服务器成功");
}

void Widget::on_registerButton_clicked()
{
    QString username = ui->userLineEdit->text();
    QString password = ui->passwdLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd", "register");
    obj.insert("user", username);
    obj.insert("password", password);

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Widget::server_reply()
{
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();
    if (cmd == "register_reply")
    {
        client_register_handler(obj.value("result").toString());
    }
    else if (cmd == "login_reply")
    {
        client_login_handler(obj.value("result").toString(),
                      obj.value("friend").toString(), obj.value("group").toString());
    }
}

void Widget::client_register_handler(QString res)
{
    if (res == "success")
    {
        QMessageBox::information(this, "注册提示", "注册成功");
    }
    else if (res == "failure")
    {
        QMessageBox::warning(this, "注册提示", "注册失败");
    }
}

void Widget::on_loginButton_clicked()
{
    QString username = ui->userLineEdit->text();
    QString password = ui->passwdLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd", "login");
    obj.insert("user", username);
    obj.insert("password", password);

    userName = username;

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Widget::client_login_handler(QString res, QString fri, QString group)
{
    if (res == "user_not_exist")
    {
        QMessageBox::warning(this, "登录提示", "用户不存在");
    }
    else if (res == "password_error")
    {
        QMessageBox::warning(this, "登录提示", "密码错误");
    }
    else if (res == "success")
    {
        this->hide();
        socket->disconnect(SIGNAL(readyRead()));
        Chatlist *c = new Chatlist(socket, fri, group, userName);
        c->setWindowTitle(userName);
        c->show();
    }
}
