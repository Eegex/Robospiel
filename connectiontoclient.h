#ifndef CONNECTIONTOCLIENT_H
#define CONNECTIONTOCLIENT_H

#include <QtNetwork/QTcpSocket>

#include <QDataStream>
#include <QObject>

class ConnectionToClient: public QObject
{
    Q_OBJECT
public:
    void sendMessage(QString message);
    ConnectionToClient(QObject *parent, QTcpSocket *tcpSocket);
private:
    QTcpSocket* tcpSocket;
    QDataStream streamFromClient;

signals:
    void receivedMessage(QString message);

private slots:
    void receiveMessage();
};

#endif // CONNECTIONTOCLIENT_H
