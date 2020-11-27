#include "networkview.h"
#include "server.h"
#include "client.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

NetworkView::NetworkView(QWidget *parent) : QWidget(parent)
{
    QPushButton* btnStartServer = new QPushButton("Start Server");
    connect(btnStartServer, &QPushButton::clicked, this, &NetworkView::addServer);
    layout->addWidget(btnStartServer);

    layout->addWidget(leMessageToClients);
    QPushButton* btnSendToClients = new QPushButton("Send Message to all clients");
    connect(btnSendToClients, &QPushButton::clicked, this, &NetworkView::sendToClients);
    layout->addWidget(btnSendToClients);


    layout->addWidget(new QLabel("________________"));
    layout->addWidget(leServerAddress);
    layout->addWidget(leServerPort);
    QPushButton* btnStartClient = new QPushButton("Start Client");
    connect(btnStartClient, &QPushButton::clicked, this, &NetworkView::addClient);
    layout->addWidget(btnStartClient);

    layout->addWidget(leMessageToServer);
    QPushButton* btnSendToServer = new QPushButton("Send Message to server");
    connect(btnSendToServer, &QPushButton::clicked, this, &NetworkView::sendToServer);
    layout->addWidget(btnSendToServer);

    setLayout(layout);
    setWindowTitle(tr("Hi"));

}

void NetworkView::addServer()
{
    server = new Server();
    qDebug()<<"view added server";
}
void NetworkView::sendToClients()
{
    server->sendMessageToClients(leMessageToClients->text());
}

void NetworkView::addClient()
{
    client=new Client(this, leServerAddress->text(), leServerPort->text().toInt());
    qDebug()<<"view added client";
}
void NetworkView::sendToServer()
{
    client->sendMessageToServer(leMessageToServer->text());
}


