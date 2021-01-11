#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include "Direction.h"


class Client : public QObject
{
    Q_OBJECT
public:
    static Client& getInstance();
    static void deleteInstance();

    void startClient(QString serverAddress, int serverPort);
    bool sendMessageToServer(QJsonObject data);
    void closeClient();
    bool isActive();
private:
    Client(QObject *parent = nullptr);
    ~Client();
    static Client instance;
    static QTcpSocket* tcpSocket;
    static QDataStream streamFromServer;

signals:
    void errorInClient(QAbstractSocket::SocketError socketError);
    void clientIsStarting();
    void clientStarted();
    void clientClosed();
    void actionReceived(QJsonObject data);

private slots:
    void processMessageFromServer();
};

#endif // CLIENT_H
