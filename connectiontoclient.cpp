#include "connectiontoclient.h"

#include <QString>

ConnectionToClient::ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket): QObject(parent),tcpSocket(tcpSocket)
{
	streamFromClient.setDevice(tcpSocket);
	connect(tcpSocket, &QIODevice::readyRead, this, &ConnectionToClient::receiveMessage);
	connect(tcpSocket, &QAbstractSocket::disconnected, this, [=]()->void{emit deleteConnection(this);});

}

void ConnectionToClient::setUser(User *value)
{
    user = value;
}

User * ConnectionToClient::getUser() const
{
    return user;
}

void ConnectionToClient::receiveMessage()
{
    streamFromClient.startTransaction();

	QString message;
	streamFromClient >> message;

    if (!streamFromClient.commitTransaction())
    {
        return;
    }
    emit receivedMessage(message);

    //process the next message, which might have be blocked by the current one, which was to long to be transmitted at once
    receiveMessage();
}

bool ConnectionToClient::sendMessage(QString message)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);

	out << message;

	return (tcpSocket->write(block)!=-1);
}
