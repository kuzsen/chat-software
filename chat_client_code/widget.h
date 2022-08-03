#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include "chatlist.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void connect_success();
    void server_reply();

    void on_registerButton_clicked();

    void on_loginButton_clicked();

private:
    void client_register_handler(QString);
    void client_login_handler(QString, QString, QString);

    Ui::Widget *ui;
    QTcpSocket *socket;
    QString userName;
};

#endif // WIDGET_H
