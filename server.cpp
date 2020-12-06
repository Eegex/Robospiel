#include "server.h"

QTcpServer* Server::server = new QTcpServer();
Server Server::instance;
QVector<ConnectionToClient*> Server::connections;
Server::Server(QObject *parent) : QObject(parent) {}

Server& Server::getInstance()
{
    return instance;
}
void Server::deleteInstance()
{
    delete server;
    server = nullptr;
}

void Server::startServer(QString address, int port)
{
    if(server->isListening())
    {
        if(server->serverAddress().toString()==address || server->serverPort()==port)
        {
            return;
        }
        else
        {
            server->close();
        }
    }
    if (!server->listen(QHostAddress(address),port))
    {
        emit serverNotStarted();
        return;
    }

    connect(server, &QTcpServer::newConnection, &instance, &Server::addClient);
    emit serverStarted(server->serverAddress(), server->serverPort());
}

int Server::sendMessageToClients(QString message)
{
    int errorCount = 0;
    for(ConnectionToClient* client : connections)
    {
        errorCount += client->sendMessage(message) ? 0:1;
    }
    return errorCount;
}

void Server::addClient()
{
    QTcpSocket* tcpServerConnection = server->nextPendingConnection();
    if (!tcpServerConnection) {
        return;
    }

    ConnectionToClient* connection = new ConnectionToClient(this, tcpServerConnection);
    connections.append(connection);

    connect(connection, &ConnectionToClient::deleteConnection, this, [=](ConnectionToClient* toDelete)->void{
        connections.remove(connections.indexOf(toDelete));
        delete toDelete;
        toDelete = nullptr;
        emit clientsChanged(connections.length());
    });
    connect(connection, &ConnectionToClient::receivedMessage, this, &Server::processMessageFromClient);

    emit clientsChanged(connections.length());
}

void Server::processMessageFromClient(QString message)
{
    qDebug()<<"Server received: "<<message;
    //TODO handle message

}

void Server::closeServer()
{
    server->close();
    while(!connections.empty())
    {
        ConnectionToClient* toDelete = connections.takeFirst();
        delete toDelete;
        toDelete = nullptr;
    }
    emit serverClosed();
}
