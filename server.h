#ifndef SERVER_H
#define SERVER_H
#include "connectiontoclient.h"

#include <QDataStream>
#include <QObject>

#include <QtNetwork/qtcpserver.h>


class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void sendMessageToClients(QString message);
private:
    QTcpServer* server;
    QVector<ConnectionToClient*> connections;





signals:

private slots:
    void addClient();
    void processMessageFromClient(QString message);
};

#endif // SERVER_H
