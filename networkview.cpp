#include "networkview.h"
#include "server.h"
#include "client.h"
#include <QIntValidator>
#include <QLabel>
#include <QMetaEnum>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>

NetworkView::NetworkView(QWidget *parent) : QWidget(parent)
{
	QValidator *intValidator = new QIntValidator(this);

	layout = new QGridLayout(this);

	//Client gui
	clientContainer = new QWidget(this);
	gridClient=new QGridLayout(clientContainer);
	leClientAddress = new QLineEdit(clientContainer);
	leClientAddress->setPlaceholderText(tr("Server Address"));
	leClientPort = new QLineEdit(clientContainer);
	leClientPort->setPlaceholderText(tr("Server Port"));
	leClientPort->setValidator(intValidator);

	QPushButton* btnStartClient = new QPushButton(tr("Start Client"), clientContainer);
	connect(btnStartClient, &QPushButton::clicked, this, &NetworkView::addClient);
	QPushButton* btnStopClient = new QPushButton(tr("Stop Client / Back"), clientContainer);
	connect(btnStopClient, &QPushButton::clicked, this, [&]()->void {Client::getInstance().closeClient();
			toChoiceMenu();});
	clientStatus = new QLabel(clientContainer);

	gridClient->addWidget(leClientAddress);
	gridClient->addWidget(leClientPort);
	gridClient->addWidget(btnStartClient);
	gridClient->addWidget(btnStopClient);
	gridClient->addWidget(clientStatus);


	clientContainer->setLayout(gridClient);

	//Server gui
	serverContainer = new QWidget(this);
	gridServer=new QGridLayout(serverContainer);
	leServerAddress = new QLineEdit(serverContainer);
	leServerAddress->setObjectName("Nervensäge");
	leServerAddress->setPlaceholderText(tr("Server Address"));
	leServerPort = new QLineEdit(serverContainer);
	leServerPort->setPlaceholderText(tr("Server Port"));
	leServerPort->setValidator(intValidator);

	QPushButton* btnStartServer = new QPushButton(tr("Start Server"), serverContainer);
	connect(btnStartServer, &QPushButton::clicked, this, &NetworkView::addServer);
	QPushButton* btnStopServer = new QPushButton(tr("Stop Server / Back"), serverContainer);
	connect(btnStopServer, &QPushButton::clicked, this, [&]()->void
	{
		Server::getInstance().closeServer();
		toChoiceMenu();
	});
	serverStatus = new QLabel(serverContainer);

	QNetworkAccessManager * ipSearcher = new QNetworkAccessManager(this);
	connect(ipSearcher,&QNetworkAccessManager::finished,this,[=](QNetworkReply * reply)
	{
		leServerAddress->setText("acqcke5skvirioi1.myfritz.net"/*reply->readAll()*/);
		reply->deleteLater();
		ipSearcher->deleteLater();
	});
	ipSearcher->get(QNetworkRequest({"http://api64.ipify.org/"}));

	gridServer->addWidget(leServerAddress);
	gridServer->addWidget(leServerPort);
	gridServer->addWidget(btnStartServer);
	gridServer->addWidget(btnStopServer);
	gridServer->addWidget(serverStatus);

	serverContainer->setLayout(gridServer);

	btnClient = new QPushButton(tr("Join Server"));
	btnServer = new QPushButton(tr("Start Server"));
	btnOffline = new QPushButton(tr("Play offline"));
	layout->addWidget(btnClient);
	layout->addWidget(btnServer);
	layout->addWidget(btnOffline);

	setLayout(layout);

	connect(btnClient, &QPushButton::clicked, this, [=]()->void{
		layout->removeWidget(btnClient);
		layout->removeWidget(btnServer);
		layout->removeWidget(btnOffline);
		btnClient->hide();
		btnServer->hide();
		btnOffline->hide();

		layout->addWidget(clientContainer);
		clientContainer->show();
	});
	connect(btnServer, &QPushButton::clicked, this, [=]()->void{
		layout->removeWidget(btnClient);
		layout->removeWidget(btnServer);
		layout->removeWidget(btnOffline);
		btnClient->hide();
		btnServer->hide();
		btnOffline->hide();

		layout->addWidget(serverContainer);
		serverContainer->show();
	});
	connect(btnOffline, &QPushButton::clicked, this, [=]()->void{
		emit leaderboardOffline();
	});

	//connect server signals
	connect(&Server::getInstance(), &Server::serverNotStarted, this, [=]() -> void
	{
		serverStatus->setText(tr("Server couldn't start. ")+QTime::currentTime().toString());
	});
	connect(&Server::getInstance(), &Server::serverStarted, this, [=] (QHostAddress address, int port) -> void
	{
		serverStatus->setText(tr("Server is running. ")+QTime::currentTime().toString()+"\n"+address.toString()+":"+QString::number(port));
		emit leaderboardOnline();
	});
	connect(&Server::getInstance(), &Server::clientsChanged, this, [=] (int clientCount) -> void
	{
		serverStatus->setText(QString::number(clientCount) + tr(" client(s) connected to the server. ")+QTime::currentTime().toString());
	});
	connect(&Server::getInstance(), &Server::serverClosed, this, [&]()->void
	{
		serverStatus->setText(tr("Server stopped. ")+QTime::currentTime().toString());
	});

	//connect client signals
	connect(&Client::getInstance(), &Client::errorInClient, this, [&](QAbstractSocket::SocketError socketError)->void
	{
		QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
		QString errorMessage = metaEnum.valueToKey(socketError);
		clientStatus->setText(tr("Error in client: ")+errorMessage+" "+QTime::currentTime().toString());
	});
	connect(&Client::getInstance(), &Client::clientIsStarting, this, [&]()->void
	{
		clientStatus->setText(tr("Client is starting... ")+QTime::currentTime().toString());
	});
	connect(&Client::getInstance(), &Client::clientStarted, this, [&]()->void
	{
		clientStatus->setText(tr("Client started. ")+QTime::currentTime().toString());
		emit leaderboardOnline();
	});
	connect(&Client::getInstance(), &Client::clientClosed, this, [&]()->void
	{
		clientStatus->setText(tr("Client stopped. ")+QTime::currentTime().toString());
	});

}

NetworkView::~NetworkView()
{
	Server::deleteInstance();
	Client::deleteInstance();
}

void NetworkView::toChoiceMenu()
{
	if(Server::getInstance().isActive())
	{
		Server::getInstance().closeServer();
	}
	if(Client::getInstance().isActive())
	{
		Client::getInstance().closeClient();
	}

	layout->removeWidget(serverContainer);
	layout->removeWidget(clientContainer);
	serverContainer->hide();
	clientContainer->hide();

	layout->addWidget(btnClient);
	layout->addWidget(btnServer);
	layout->addWidget(btnOffline);
	btnClient->show();
	btnServer->show();
	btnOffline->show();
}

void NetworkView::addServer()
{
	Server::getInstance().startServer(leServerAddress->text(), leServerPort->text().toInt());
}

void NetworkView::addClient()
{
	Client::getInstance().startClient(leClientAddress->text(), leClientPort->text().toInt());
}



