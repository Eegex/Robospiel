#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDebug>


class Client : public QObject
{
    Q_OBJECT
public:
    static Client& getInstance();

    void startClient(QString serverAddress, int serverPort);
    bool sendMessageToServer(QString message);
    void closeClient();
private:
    Client(QObject *parent = nullptr);
    ~Client();
    static Client instance;
    static QTcpSocket* tcpSocket;
    static QDataStream streamFromServer;

signals:
    void errorInClient(QAbstractSocket::SocketError socketError);
    void clientStarted();
    void clientClosed();

private slots:
    void processMessageFromServer();
};

#endif // CLIENT_H
