#include "connectiontoclient.h"
#include <QString>
#include <chrono>

using namespace std::chrono_literals;

ConnectionToClient::ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket): QObject(parent),tcpSocket(tcpSocket)
{
	//connect(&connectionWatchDog,&QTimer::timeout,this,&ConnectionToClient::checkConnection);
	connectionWatchDog.start(15s);
	streamFromClient.setDevice(tcpSocket);
	connect(tcpSocket, &QIODevice::readyRead, this, &ConnectionToClient::receiveMessage);
	connect(tcpSocket, &QAbstractSocket::disconnected, this, [=]()->void
	{
		emit deleteConnection(this);
	});
}

void ConnectionToClient::setUser(User *value)
{
	user = value;
}

void ConnectionToClient::sendLeft()
{
	//qDebug() << "ConnectionToClient::sendLeft()";
	if(tcpSocket->bytesToWrite() > 0)
	{
		tcpSocket->waitForBytesWritten();
	}
	tcpSocket->close();
	tcpSocket->waitForDisconnected();
}

User * ConnectionToClient::getUser() const
{
	return user;
}

QTcpSocket *ConnectionToClient::getTcpSocket() const
{
	return tcpSocket;
}

void ConnectionToClient::setTcpSocket(QTcpSocket *value)
{
	tcpSocket = value;
}

void ConnectionToClient::receiveMessage()
{
	streamFromClient.startTransaction();

	QString message;
	streamFromClient >> message;
	connectionWatchDog.start();
	if(!streamFromClient.commitTransaction())
	{
		return;
	}
	if(message == "Heartbeat")
	{
		if(disconnectCheck)
		{
			//qDebug() << "Server: Heartbeat angenommen";
			disconnectCheck = false;
		}
		else
		{
			//qDebug() << "Server: antwort Heartbeat";
			sendMessage("Heartbeat");
		}
	}
	else
	{
		emit receivedMessage(message);
	}
	//process the next message, which might have be blocked by the current one, which was to long to be transmitted at once
	receiveMessage();
}

void ConnectionToClient::checkConnection()
{
	if(disconnectCheck)
	{
		//qDebug() << "Client" << user->getName() << "disconnected";
		sendLeft();
	}
	disconnectCheck = true;
	//qDebug() << "Server: sende Heartbeat";
	sendMessage("Heartbeat");
}

bool ConnectionToClient::sendMessage(QString message)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);

	out << message;

	return (tcpSocket->write(block)!=-1);
}
