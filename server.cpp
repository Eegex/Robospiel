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

/*!
 * \brief Server::deleteInstance Consider using Server::closeServer(), the sigleton shouldn't be deleted while the program is running.
 */
void Server::deleteInstance()
{
	delete server;
	server = nullptr;
}

/*!
 * \brief Server::startServer emits either serverNotStarted or serverStarted
 * \param address
 * \param port
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
	//make the server listen
	if (!server->listen(QHostAddress(address),port))
	{
		emit instance.serverNotStarted();
		return;
	}
	connect(server, &QTcpServer::newConnection, &instance, &Server::addClient,Qt::UniqueConnection);
	emit instance.serverStarted(server->serverAddress(), server->serverPort());
}

/*!
 * \brief Server::sendMessageToClients sends the additionalData to all connected clients
 * (and triggers local interpretation of additionalData as well)
 * \param additionalData
 * \return the number of clients where an error occured and the data may not have been sent correctly
 */
int Server::sendMessageToClients(QJsonObject additionalData)
{
	//prepare the message
	QJsonDocument document(additionalData);
	QString message = QString::fromUtf8(document.toJson());
	return forwardMessageToClients(message);
}

/*!
 * \brief Server::forwardMessageToClients sends the message to all connected clients
 * (and triggers local interpretation of additionalData as well)
 * \param message
 * \return the number of clients where an error occured and the data may not have been sent correctly
 */
int Server::forwardMessageToClients(QString message)
{
	int errorCount = 0;
	for(ConnectionToClient* client : qAsConst(connections))
	{
		errorCount += client->sendMessage(message) ? 0:1;
	}
	QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();
	data.insert("Client","Server");
	GameControll::addTransmission(data);
	emit instance.actionReceived(data);
	return errorCount;
}

/*!
 * \brief Server::addClient Accepts a client that tries to connect to the server, setup of mesage distribution to all other clients,
 * sends current state of the game to the new client and syncronizes the random generators.
 * Emits clientsChanged.
 */
void Server::addClient()
{
	QTcpSocket* tcpServerConnection = server->nextPendingConnection();
	if (!tcpServerConnection)
	{
		return;
	}

	ConnectionToClient* connection = new ConnectionToClient(this, tcpServerConnection);
	connections.append(connection);
	//"Das ist code für wenn ein Client einfach geht" - Dorothee
	connect(connection, &ConnectionToClient::deleteConnection, this, [=](ConnectionToClient* toDelete)->void
	{
		if(!connections.contains(toDelete))
		{
			return;
		}
		User* u = toDelete->getUser();
		connections.remove(connections.indexOf(toDelete));
		delete toDelete;
		toDelete = nullptr;
		emit clientsChanged(connections.length());
		//TODO reset local data in clients and server

		if(u)
		{
			QJsonObject data;
			data.insert("action", PlayerAction::userLeft);
			data.insert("userId", u->getId().toString());
			sendMessageToClients(data);
		}
	});
	connect(connection, &ConnectionToClient::receivedMessage, this, [this](QString message)
	{
		ConnectionToClient * senderConnection = dynamic_cast<ConnectionToClient*>(sender()); //get connection over sender instead of capturing it to prevent stupid behavior
		Q_ASSERT_X(senderConnection,"Server::addClient() receivedMessage-lambda","senderConnection is nullptr");
		QJsonObject data = QJsonDocument::fromJson(message.toUtf8()).object();
		//network debugging
		User * u = senderConnection->getUser();
		if(u)
		{
			data.insert("Client","S: " + u->getName());
		}
		else
		{
			data.insert("Client","-");
		}
		GameControll::addTransmission(data);


		PlayerAction action = static_cast<PlayerAction>(data.value("action").toInt());
		if(action == PlayerAction::registerClient)
		{
			senderConnection->setUser(User::fromJSON(data));

		}
		else
		{
			Server::forwardMessageToClients(message);
		}


	});

	//send board and users to new client
	QJsonObject state = GameControll::toJSON();
	state.insert("action", PlayerAction::completeUpdate);
	QJsonDocument document(state);
	connection->sendMessage(QString::fromUtf8(document.toJson()));

	emit clientsChanged(connections.length());

	GameControll::triggerActionWithData(PlayerAction::syncRandomGenerators,{{"Seed",QTime::currentTime().msecsSinceStartOfDay()}});
}

/**
 * @brief Server::cleanupUsers
 * Searches for a Connection for each user in GameControll to eliminate users, that didn't reconnect after a serverswitch.
 */
void Server::cleanupUsers()
{
	for(User* u : *(GameControll::getUsers()))
	{
		bool foundConnection = false;
		for(ConnectionToClient* c : connections)
		{
			if(c->getUser() && c->getUser()->getId()==u->getId())
			{
				foundConnection = true;
				break;
			}
		}
		//if !foundConnection && man ist nicht selber der server
		if(!foundConnection && (!GameControll::getLocalUser() || u->getId()!=GameControll::getLocalUser()->getId()))
		{
			emit instance.clientsChanged(GameControll::getUsers()->size());
			QJsonObject data;
			data.insert("action", PlayerAction::userLeft);
			data.insert("userId", u->getId().toString());
			sendMessageToClients(data);
		}
	}
}

void Server::closeServer()
{
	while(!connections.empty())
	{
		ConnectionToClient * toDelete = connections.takeFirst();
		toDelete->sendLeft();
		toDelete->deleteLater();
	}
	server->close();
	emit instance.serverClosed();
}

/**
 * @brief Server::switchServer
 * @param restart determines if the old server reconnects after the switch
 */
void Server::switchServer(bool restart)
{
	if(!connections.empty())
	{
		ConnectionToClient* newServer = connections.first();
		QUuid id = newServer->getUser()->getId();
		//QString ip = "localhost";
		int port = 8050;

		QHostAddress ip = newServer->getTcpSocket()->peerAddress();
		//int port = newServer->getTcpSocket()->peerPort();

		GameControll::triggerActionWithData(PlayerAction::switchServer,{{"port", port}, {"id", id.toString()}, {"ip", ip.toString()}, {"restartOldServer", restart}});
	}
	//test
}

int Server::connectedClientCount()
{
	return instance.connections.size();
}

bool Server::isActive()
{
	return Server::server->isListening();
}
