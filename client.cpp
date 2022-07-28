#include "client.h"
#include "userview.h"
#include "gamecontroll.h"
#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonObject>
#include <chrono>

using namespace std::chrono_literals;

//comment for helenas git

Client Client::instance;
QDataStream Client::streamFromServer;
bool Client::connected = false;
QTimer Client::connectionWatchDog;
bool Client::disconnectCheck = false;
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
	if(!connected)
	{
		connected = true;
		connect(tcpSocket, &QIODevice::readyRead, this, &Client::processMessageFromServer);
		connect(tcpSocket, &QAbstractSocket::connected, this, [=]()-> void
		{
			emit clientStarted();
		});
		connect(tcpSocket, &QAbstractSocket::disconnected, this, [=]()->void
		{
			emit clientClosed();
		});
#if QT_VERSION_MAJOR == 5
		connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [=](QAbstractSocket::SocketError socketError) -> void {emit errorInClient(socketError);});
#elif QT_VERSION_MAJOR == 6
		connect(tcpSocket, &QAbstractSocket::errorOccurred, this, [=](QAbstractSocket::SocketError socketError) -> void
		{
			emit errorInClient(socketError);
		});
#endif
	}
	emit clientIsStarting();
	tcpSocket->close();
	streamFromServer.setDevice(tcpSocket);
	connect(&connectionWatchDog,&QTimer::timeout,&instance,&Client::checkConnection,Qt::UniqueConnection);
	disconnectCheck = false;
	tcpSocket->connectToHost(serverAddress, serverPort);
	tcpSocket->waitForConnected();
	connectionWatchDog.start(15s);
}

bool Client::sendMessageToServer(QJsonObject data)
{
	//prepare the message
	QJsonDocument document(data);
	QString message = QString::fromUtf8(document.toJson());

	data.insert("Client",GameControll::getLocalUserName());
	GameControll::addTransmission(data);
	return sendMessage(message);
}

void Client::processMessageFromServer()
{
	QString message;
	streamFromServer.startTransaction();
	streamFromServer >> message;
	if(connected)
	{
		connectionWatchDog.start();
	}
	if(!streamFromServer.commitTransaction())
	{
		return;
	}
	if(message == "Heartbeat")
	{
		if(disconnectCheck)
		{
			disconnectCheck = false;
		}
		else
		{
			sendMessage("Heartbeat");
		}
	}
	else
	{
		QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();

		data.insert("Client","Server");
		GameControll::addTransmission(data);
		emit actionReceived(data);
	}
	//process the next message, which might have be blocked by the current one, which was to long to be transmitted at once
	processMessageFromServer();
}

void Client::checkConnection()
{
	if(disconnectCheck)
	{
		UserView::disconnectFromServer();
		connectionWatchDog.stop();
	}
	disconnectCheck = true;
	sendMessage("Heartbeat");
}

void Client::closeClient()
{
	streamFromServer.abortTransaction();
	streamFromServer.resetStatus();
	instance.tcpSocket->close();
	connected = false;
	disconnectCheck = false;
	connectionWatchDog.stop();
}

bool Client::isActive()
{
	return (tcpSocket->state()==QAbstractSocket::BoundState) ||(tcpSocket->state()==QAbstractSocket::ConnectedState);
}

Client::~Client()
{
	delete tcpSocket; //closes the connection
}

bool Client::sendMessage(QString message)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out << message;
	return tcpSocket->isOpen() && (tcpSocket->write(block)!=-1);
}
