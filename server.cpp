#include "server.h"
#include "Direction.h"
#include "gamecontroll.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

QTcpServer* Server::server = new QTcpServer();
Server Server::instance;
QVector<ConnectionToClient*> Server::connections;
Server::Server(QObject *parent) : QObject(parent) {}

Server& Server::getInstance()
{
	return instance;
}

/**
 * @brief Server::deleteInstance Consider using Server::closeServer(), the sigleton shouldn't be deleted while the program is running.
 */
void Server::deleteInstance()
{
	delete server;
	server = nullptr;
}

/**
 * @brief Server::startServer emits either serverNotStarted or serverStarted
 * @param address
 * @param port
 */
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
        emit instance.serverNotStarted();
		return;
	}

	connect(server, &QTcpServer::newConnection, &instance, &Server::addClient);
    emit instance.serverStarted(server->serverAddress(), server->serverPort());
}

/**
 * @brief Server::sendMessageToClients sends the additionalData to all connected clients
 * (and triggers local interpretation of additionalData as well)
 * @param additionalData
 * @return the number of clients where an error occured and the data may not have been sent correctly
 */
int Server::sendMessageToClients(QJsonObject additionalData)
{
	//prepare the message
	QJsonDocument document(additionalData);
	QString message = QString::fromUtf8(document.toJson());
	return forwardMessageToClients(message);
}

/**
 * @brief Server::forwardMessageToClients sends the message to all connected clients
 * (and triggers local interpretation of additionalData as well)
 * @param message
 * @return the number of clients where an error occured and the data may not have been sent correctly
 */
int Server::forwardMessageToClients(QString message)
{
	int errorCount = 0;
	for(ConnectionToClient* client : qAsConst(connections))
	{
		errorCount += client->sendMessage(message) ? 0:1;
	}

	QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();
    emit instance.actionReceived(data);
	return errorCount;
}

/**
 * @brief Server::addClient Accepts a client that tries to connect to the server, setup of mesage distribution to all other clients,
 * sends current state of the game to the new client and syncronizes the random generators.
 * Emits clientsChanged.
 */
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
//        emit clientDeconnected(); TODO
	});
    connect(connection, &ConnectionToClient::receivedMessage, this, [&](QString message){
        QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();
        PlayerAction action = static_cast<PlayerAction>(data.value("action").toInt());
        if(action==PlayerAction::registerClient)
        {
                connection->setUser(User::fromJSON(data));
        }
        Server::forwardMessageToClients(message);
    });

	//send board and users to new client
	QJsonObject state = GameControll::toJSON();
	state.insert("action", PlayerAction::completeUpdate);
	QJsonDocument document(state);
	connection->sendMessage(QString::fromUtf8(document.toJson()));

	emit clientsChanged(connections.length());

	QJsonObject seed;
	seed.insert("Seed",QTime::currentTime().msecsSinceStartOfDay());

	GameControll::triggerActionWithData(PlayerAction::syncRandomGenerators,seed);
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
    emit instance.serverClosed();
}

bool Server::isActive()
{
	return Server::server->isListening();
}
