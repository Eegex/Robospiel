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

    ~Client();
    void sendMessageToServer(QString message);
    Client(QObject *parent, QString serverAddress, int serverPort);
private:
        QTcpSocket* tcpSocket;
        QDataStream streamFromServer;

signals:

private slots:
        void displayError(QAbstractSocket::SocketError socketError);
        void processMessageFromServer();
};

#endif // CLIENT_H
