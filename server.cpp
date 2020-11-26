
#include "server.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/qnetworkinterface.h>

#include <QDataStream>

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    if (!server->listen())
    {
        qDebug()<<"Starting the server failed. ";
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
        {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
    {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    qDebug()<<"Server running: "<<ipAddress<<":"<<server->serverPort();

    connect(server, &QTcpServer::newConnection, this, &Server::addClient);
}

void Server::addClient()
{
    QTcpSocket* tcpServerConnection = server->nextPendingConnection();
    if (!tcpServerConnection) {
        qDebug()<<"Error: got invalid pending connection!";
        return;
    }

    ConnectionToClient* connection = new ConnectionToClient(this, tcpServerConnection);
    connections.append(connection);

    connect(tcpServerConnection, &QTcpSocket::disconnected,
            tcpServerConnection, &QTcpSocket::deleteLater); //todo delete the struct completely
    connect(connection, &ConnectionToClient::receivedMessage, this, &Server::processMessageFromClient);



    qDebug()<<"New Client connected to server";
}

void Server::sendMessageToClients(QString message)
{
    for(ConnectionToClient* client : connections)
    {
        client->sendMessage(message);
    }
}

void Server::processMessageFromClient(QString message)
{
    qDebug()<<"Server received: "<<message;
}
