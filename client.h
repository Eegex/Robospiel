#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDebug>
#include <QJsonObject>


class Client : public QObject
{
	Q_OBJECT
public:
	static Client& getInstance();
	static void deleteInstance();

	void startClient(QString serverAddress, int serverPort);
	bool sendMessageToServer(QJsonObject data);
	static void closeClient();
	static bool isActive();
private:
	Client(QObject *parent = nullptr);
	~Client();
	bool sendMessage(QString message);
	static Client instance;
	static QTcpSocket* tcpSocket;
	static QDataStream streamFromServer;
	static bool connected;
	static QTimer connectionWatchDog;
	static bool disconnectCheck;
signals:
	void errorInClient(QAbstractSocket::SocketError socketError);
	void clientIsStarting();
	void clientStarted();
	void clientClosed();
	void actionReceived(QJsonObject data);

private slots:
	void processMessageFromServer();
	void checkConnection();
};

#endif // CLIENT_H
