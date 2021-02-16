#include "connectiontoclient.h"

#include <QString>

ConnectionToClient::ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket): QObject(parent),tcpSocket(tcpSocket)
{
	streamFromClient.setDevice(tcpSocket);
	connect(tcpSocket, &QIODevice::readyRead, this, &ConnectionToClient::receiveMessage);
	connect(tcpSocket, &QAbstractSocket::disconnected, this, [=]()->void{emit deleteConnection(this);});

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
	receiveMessage();
}

bool ConnectionToClient::sendMessage(QString message)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);

	out << message;

	return (tcpSocket->write(block)!=-1);
}
