#include "groupchat.h"
#include "ui_groupchat.h"

GroupChat::GroupChat(QTcpSocket *s, QString g, QString u, Chatlist *c, QList<groupWidgetInfo> *l, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupChat)
{
    ui->setupUi(this);
    socket = s;
    groupName = g;
    userName = u;
    mainWidget = c;
    groupWidgetList = l;

    QJsonObject obj;
    obj.insert("cmd", "get_group_member");
    obj.insert("group", groupName);
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    connect(mainWidget, &Chatlist::signal_to_sub_widget_member, this, &GroupChat::show_group_member);
    connect(mainWidget, &Chatlist::signal_to_sub_widget_group, this, &GroupChat::show_group_text);
}

GroupChat::~GroupChat()
{
    delete ui;
}

void GroupChat::show_group_member(QJsonObject obj)
{
    if (obj.value("cmd").toString() == "get_group_member_reply")
    {
        if (obj.value("group").toString() == groupName)
        {
            QStringList strList = obj.value("member").toString().split("|");
            for (int i = 0; i < strList.size(); i++)
            {
                ui->listWidget->addItem(strList.at(i));
            }
        }
    }
}

void GroupChat::on_sendButton_clicked()
{
    QString text = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "group_chat");
    obj.insert("user", userName);
    obj.insert("group", groupName);
    obj.insert("text", text);
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    ui->lineEdit->clear();
    ui->textEdit->append(text);
    ui->textEdit->append("\n");
}

void GroupChat::show_group_text(QJsonObject obj)
{
    if (obj.value("cmd").toString() == "group_chat")
    {
        if (obj.value("group").toString() == groupName)
        {
            if (this->isMinimized())
            {
                this->showNormal();
            }
            this->activateWindow();
            ui->textEdit->append(obj.value("text").toString());
            ui->textEdit->append("\n");
        }
    }
}

void GroupChat::closeEvent(QCloseEvent * event)
{
    for (int i = 0; i < groupWidgetList->size(); i++)
    {
        if (groupWidgetList->at(i).name == groupName)
        {
            groupWidgetList->removeAt(i);
        }
    }
    event->accept();
}
