#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class CreateGroup;
}

class CreateGroup : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGroup(QTcpSocket *s, QString u, QWidget *parent = 0);
    ~CreateGroup();

private slots:
    void on_cancelButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CreateGroup *ui;
    QString userName;
    QTcpSocket *socket;
};

#endif // CREATEGROUP_H
