#ifndef SERVER_H
#define SERVER_H
#include "connectiontoclient.h"
#include <QDataStream>
#include <QObject>
#include <QtNetwork>
#include <QtCore>
#include <QVector>

class Server : public QObject
{
    Q_OBJECT
public:
    static Server& getInstance();

    void startServer(QString address, int port);
    int sendMessageToClients(QString message);
    void closeServer();
private:
    Server(QObject *parent = nullptr);
    static Server instance;
    static QTcpServer* server;
    static QVector<ConnectionToClient*> connections;
signals:
    void serverNotStarted();
    void serverStarted(QHostAddress address, int port);
    void clientsChanged(int clientCount);

private slots:
    void addClient();
    void processMessageFromClient(QString message);
};

#endif // SERVER_H
