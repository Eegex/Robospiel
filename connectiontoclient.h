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

	void sendLeft();

	User * getUser() const;

    QTcpSocket *getTcpSocket() const;
    void setTcpSocket(QTcpSocket *value);

private:
    QTcpSocket * tcpSocket = nullptr;
    QDataStream streamFromClient;
	User * user = nullptr;

signals:
	void receivedMessage(QString message);
	void deleteConnection(ConnectionToClient* self);

private slots:
	void receiveMessage();
};

#endif // CONNECTIONTOCLIENT_H
