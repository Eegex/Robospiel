#ifndef CONNECTIONTOCLIENT_H
#define CONNECTIONTOCLIENT_H

#include <QtNetwork/QTcpSocket>

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

	User * getUser() const;

private:
	QTcpSocket* tcpSocket;
	QDataStream streamFromClient;
	User* user;

signals:
	void receivedMessage(QString message);
	void deleteConnection(ConnectionToClient* self);

private slots:
	void receiveMessage();
};

#endif // CONNECTIONTOCLIENT_H
