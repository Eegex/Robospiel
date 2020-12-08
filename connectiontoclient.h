#ifndef CONNECTIONTOCLIENT_H
#define CONNECTIONTOCLIENT_H

#include <QtNetwork/QTcpSocket>

#include <QDataStream>
#include <QObject>

class ConnectionToClient: public QObject
{
    Q_OBJECT
public:
    bool sendMessage(QString message);
    ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket);
private:
    QTcpSocket* tcpSocket;
    QDataStream streamFromClient;

signals:
    void receivedMessage(QString message);
    void deleteConnection(ConnectionToClient* self);

private slots:
    void receiveMessage();
};

#endif // CONNECTIONTOCLIENT_H
