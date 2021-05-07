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

	void startServer(QString address, int port);
	int sendMessageToClients(QJsonObject additionalData);
	void closeServer();
	static bool isActive();
private:
	Server(QObject *parent = nullptr);
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
	int forwardMessageToClients(QString message);
};

#endif // SERVER_H
