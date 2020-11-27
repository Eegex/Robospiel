#include "connectiontoclient.h"

ConnectionToClient::ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket): QObject(parent),tcpSocket(tcpSocket)
{
    streamFromClient.setDevice(tcpSocket);
    connect(tcpSocket, &QIODevice::readyRead, this, &ConnectionToClient::receiveMessage);

}

void ConnectionToClient::receiveMessage()
{
    streamFromClient.startTransaction();

    QString message;
    streamFromClient >> message;

    if (!streamFromClient.commitTransaction())
        return;

    qDebug()<<"Connection received: "<<message;
    emit receivedMessage(message);
}

void ConnectionToClient::sendMessage(QString message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << message;

    tcpSocket->write(block);
}
