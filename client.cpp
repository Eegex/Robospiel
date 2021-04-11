#include "client.h"

#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonObject>

Client Client::instance;
QDataStream Client::streamFromServer;
QTcpSocket* Client::tcpSocket = new QTcpSocket();
Client::Client(QObject *parent) : QObject(parent) {}

Client& Client::getInstance()
{
	return instance;
}
void Client::deleteInstance()
{
	delete tcpSocket;
	tcpSocket = nullptr;
}

void Client::startClient(QString serverAddress, int serverPort)
{
	emit clientIsStarting();
	tcpSocket->close();
	streamFromServer.setDevice(tcpSocket);
	connect(tcpSocket, &QIODevice::readyRead, this, &Client::processMessageFromServer);
	connect(tcpSocket, &QAbstractSocket::connected, this, [=]()-> void{emit clientStarted();});
	connect(tcpSocket, &QAbstractSocket::disconnected, this, [=]()->void{emit clientClosed();});
	connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [=](QAbstractSocket::SocketError socketError) -> void {emit errorInClient(socketError);});
	//connect(tcpSocket, QAbstractSocket::errorOccurred, this, [=](QAbstractSocket::SocketError socketError) -> void {emit errorInClient(socketError);});

	tcpSocket->connectToHost(serverAddress, serverPort);
}

bool Client::sendMessageToServer(QJsonObject data)
{
	//prepare the message
	QJsonDocument document(data);
	QString message = QString::fromUtf8(document.toJson());

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out << message;

	if(tcpSocket->isOpen())
	{
		return (tcpSocket->write(block)!=-1);
	}
	return false;

}

void Client::processMessageFromServer()
{
    QString message;
    streamFromServer.startTransaction();
    streamFromServer >> message;

    if (!streamFromServer.commitTransaction())
    {
        return;
    }

    QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();
    emit actionReceived(data);

    //process the next message, which might have be blocked by the current one, which was to long to be transmitted at once
    processMessageFromServer();
}

void Client::closeClient()
{
	tcpSocket->close();
}

bool Client::isActive()
{
	return (tcpSocket->state()==QAbstractSocket::BoundState) ||(tcpSocket->state()==QAbstractSocket::ConnectedState);
}

Client::~Client()
{
	delete tcpSocket; //closes the connection
}
