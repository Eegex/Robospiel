#ifndef CONNECTIONTOCLIENT_H
#define CONNECTIONTOCLIENT_H

#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QDataStream>
#include <QObject>
#include "user.h"

class ConnectionToClient: public QObject
{
	Q_OBJECT
public:
	bool sendMessage(QString message);
	ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket);
	void setUser(User *value);

	void sendLeft();

	User * getUser() const;

	QTcpSocket *getTcpSocket() const;
	void setTcpSocket(QTcpSocket *value);

private:
	QTcpSocket * tcpSocket = nullptr;
	QDataStream streamFromClient;
	User * user = nullptr;
	QTimer connectionWatchDog;
	bool disconnectCheck = false;
signals:
	void receivedMessage(QString message);
	void deleteConnection(ConnectionToClient* self);

private slots:
	void receiveMessage();
	void checkConnection();
};

#endif // CONNECTIONTOCLIENT_H
