#include "server.h"
#include "Direction.h"
#include "gamecontroll.h"

#include <QJsonDocument>
#include <QJsonObject>

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

int Server::sendMessageToClients(QJsonObject additionalData)
{
	//prepare the message
	QJsonDocument document(additionalData);
	QString message = QString::fromUtf8(document.toJson());
	return forwardMessageToClients(message);
}

int Server::forwardMessageToClients(QString message)
{
	int errorCount = 0;
	for(ConnectionToClient* client : connections)
	{
		errorCount += client->sendMessage(message) ? 0:1;
	}

	QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();
	emit actionReceived(data);
	return errorCount;
}

void Server::addClient()
{
	qDebug()<<"Added new Client to Server";
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
	connect(connection, &ConnectionToClient::receivedMessage, this, &Server::forwardMessageToClients);


	//send board and users to new client
	QJsonObject state = GameControll::toJSON();
	state.insert("action", PlayerAction::completeUpdate);
	QJsonDocument document(state);
	connection->sendMessage(QString::fromUtf8(document.toJson()));

    emit clientsChanged(connections.length());
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

bool Server::isActive()
{
	return Server::server->isListening();
}
