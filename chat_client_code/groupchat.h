#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>

class Chatlist;
struct groupWidgetInfo;
#include "chatlist.h"

namespace Ui {
class GroupChat;
}

class GroupChat : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChat(QTcpSocket *s, QString g, QString u, Chatlist *c, QList<groupWidgetInfo> *l, QWidget *parent = 0);
    ~GroupChat();
    void closeEvent(QCloseEvent *);

private slots:
    void show_group_member(QJsonObject);

    void on_sendButton_clicked();

    void show_group_text(QJsonObject);

private:
    Ui::GroupChat *ui;
    QTcpSocket *socket;
    QString userName;
    QString groupName;
    Chatlist *mainWidget;
    QList<groupWidgetInfo> *groupWidgetList;
};

#endif // GROUPCHAT_H
