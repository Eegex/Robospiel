#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QObject>
#include <QtNetwork/qtcpserver.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    ~Client();
    void sendMessageToServer(QString message);
private:
        QTcpSocket* tcpSocket;
        QString serverAddress;
        int serverPort;
        QDataStream streamFromServer;

signals:

private slots:
        void displayError(QAbstractSocket::SocketError socketError);
        void processMessageFromServer();
};

#endif // CLIENT_H
