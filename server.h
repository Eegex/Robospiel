#ifndef SERVER_H
#define SERVER_H
#include "connectiontoclient.h"
#include <QDataStream>
#include <QObject>
#include <QtNetwork>
#include <QtCore>
#include <QVector>
#include "Direction.h"

class Server : public QObject
{
	Q_OBJECT
public:
	static Server& getInstance();
	static void deleteInstance();

	static void startServer(QString address, int port);
	static int sendMessageToClients(QJsonObject additionalData);
	static void closeServer();
	static bool isActive();
	static void switchServer(bool restart = true);
	static int connectedClientCount();
	static void cleanupUsers();
private:
	Server(QObject *parent = nullptr);
	static int forwardMessageToClients(QString message);
	static Server instance;
	static QTcpServer* server;
	static QVector<ConnectionToClient*> connections;
signals:
	void serverNotStarted();
	void serverStarted(QHostAddress address, int port);
	void clientsChanged(int clientCount);
	void serverClosed();
	void actionReceived(QJsonObject data);

private slots:
	void addClient();

};

#endif // SERVER_H
