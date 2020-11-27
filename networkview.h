#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include "client.h"
#include "server.h"

#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

class NetworkView : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkView(QWidget *parent = nullptr);
    QVBoxLayout* layout = new QVBoxLayout();
    QLineEdit* leServerAddress = new QLineEdit("192.168.178.27");
    QLineEdit* leServerPort = new QLineEdit("8088");
    QLineEdit* leMessageToServer = new QLineEdit("Message to server");
    QLineEdit* leMessageToClients = new QLineEdit("Message to clients");
private:
    Server* server;
    Client* client;

signals:

private slots:
    void addServer();
    void addClient();
    void sendToClients();
    void sendToServer();
};

#endif // NETWORKVIEW_H
