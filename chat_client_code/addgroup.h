#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class AddGroup;
}

class AddGroup : public QWidget
{
    Q_OBJECT

public:
    explicit AddGroup(QTcpSocket *s, QString u, QWidget *parent = 0);
    ~AddGroup();

private slots:
    void on_cancelButton_clicked();

    void on_addButton_clicked();

private:
    Ui::AddGroup *ui;
    QTcpSocket *socket;
    QString userName;
};

#endif // ADDGROUP_H
