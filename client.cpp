#include "client.h"

#include <QtNetwork/QTcpSocket>

Client::Client(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    streamFromServer.setDevice(tcpSocket);
//    streamFromServer.setVersion(QDataStream::Qt_4_0);
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::processMessageFromServer);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::displayError);
    tcpSocket->connectToHost(serverAddress, serverPort);

}

void Client::sendMessageToServer(QString message)
{
    tcpSocket->write(message.toUtf8());
    qDebug()<<"Sended message to server: "<<message;
}

void Client::processMessageFromServer()
{
    streamFromServer.startTransaction();

    QString message;
    streamFromServer >> message;

    if (!streamFromServer.commitTransaction())
        return;

    qDebug()<<"Client received: "<<message;

}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug()<<"Error in client: "<<socketError;
}

Client::~Client()
{
    delete tcpSocket; //closes the connection
}
