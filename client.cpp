#include "client.h"



Client::Client(QObject *parent, QString serverAddress, int serverPort) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    streamFromServer.setDevice(tcpSocket);
//    streamFromServer.setVersion(QDataStream::Qt_4_0);
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::processMessageFromServer);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::displayError);
    tcpSocket->connectToHost(serverAddress, serverPort);
    qDebug()<<"end client constructor";

}

void Client::sendMessageToServer(QString message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << message;

    tcpSocket->write(block);
    qDebug()<<"Sended message to server: "<<message;
}

void Client::processMessageFromServer()
{
    qDebug()<<"start of message";
    streamFromServer.startTransaction();

    QString message;
    streamFromServer >> message;
    qDebug()<<"middle"<<message;

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
