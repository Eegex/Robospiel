#include "client.h"
#include "gamecontroll.h"
#include "offlineleaderboardwidget.h"
#include "onlineleaderboardwidget.h"
#include "server.h"
#include "stackwidget.h"

StackWidget::StackWidget(QWidget *parent) : QWidget{parent}
{	
	createNetworkSelection();
	createClientLayout();
	createServerLayout();

	stack = new QStackedLayout(this);
	stack->addItem(networkSelection);
	stack->addItem(clientLayout);
	stack->addItem(serverLayout);

	backButton = new QPushButton(tr("Back"), this);

	layout = new QVBoxLayout(this);
	layout->addLayout(stack);
	layout->addWidget(backButton);

	setLayout(layout);

	connect(&GameControll::getInstance(), &GameControll::disconnectFromServer, this, &StackWidget::disconnectFromServer);

	connect(backButton, &QPushButton::clicked, this, [=]()
	{
		disconnectFromServer();
		stack->setCurrentIndex(0);
	});

	connect(stack, &QStackedLayout::currentChanged, this, [=](int index)
	{
		backButton->setVisible(index!=0);
	});
}

void StackWidget::disconnectFromServer()
{
	if(leaderboard)
	{
		leaderboard->deleteLater();
		GameControll::clearUsers();
	}

	if(Client::isActive())
	{
		Client::closeClient();
	}
	if(Server::isActive())
	{
		Server::closeServer();
	}
}

void StackWidget::createNetworkSelection()
{

	btnClient = new QPushButton(tr("Join Server"));
	btnClient->setObjectName("btnClient");
	btnServer = new QPushButton(tr("Start Server"));
	btnServer->setObjectName("btnServer");
	btnOffline = new QPushButton(tr("Play offline"));
	btnOffline->setObjectName("btnOffline");

	networkSelection = new QVBoxLayout(this);
	networkSelection->addWidget(btnClient);
	networkSelection->addWidget(btnServer);
	networkSelection->addWidget(btnOffline);


	connect(btnClient, &QPushButton::clicked, this, [=]()
	{
		stack->setCurrentIndex(1);
	});
	connect(btnServer, &QPushButton::clicked, this, [=]()
	{
		stack->setCurrentIndex(2);
	});
	connect(btnOffline, &QPushButton::clicked, this, [=]()
	{
		stack->setCurrentIndex(3);
	});

}

void StackWidget::createClientLayout()
{
	clientLayout = new QVBoxLayout(this);
	leClientAddress = new QLineEdit(defaultServer,this);
	leClientAddress->setPlaceholderText(tr("Server Address"));
	leClientPort = new QLineEdit("8050",this);
	leClientPort->setPlaceholderText(tr("Server Port"));
	leClientPort->setValidator(&intValidator);

	btnStartClient = new QPushButton(tr("Start Client"), this);
	connect(btnStartClient, &QPushButton::clicked, this, [=]()
	{
		Client::getInstance().startClient(leClientAddress->text(), leClientPort->text().toInt());
	});


	clientStatus = new QLabel(this);

	clientLayout->addWidget(leClientAddress);
	clientLayout->addWidget(leClientPort);
	clientLayout->addWidget(btnStartClient);
	clientLayout->addWidget(clientStatus);

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
		handleLeaderboardOnline();
	});
	connect(&Client::getInstance(), &Client::clientClosed, this, [&]()->void
	{
		clientStatus->setText(tr("Client stopped. ")+QTime::currentTime().toString());
	});

}

void StackWidget::createServerLayout()
{
	serverLayout = new QVBoxLayout(this);
	leServerAddress = new QLineEdit(this);
	leServerAddress->setPlaceholderText(tr("Server Address"));
	leServerPort = new QLineEdit(this);
	leServerPort->setPlaceholderText(tr("Server Port"));
	leServerPort->setValidator(&intValidator);

	btnStartServer = new QPushButton(tr("Start Server"), this);
	connect(btnStartServer, &QPushButton::clicked, this, [=]()
	{
		Server::getInstance().startServer(leServerAddress->text(), leServerPort->text().toInt());
	});
	serverStatus = new QLabel(this);

	QNetworkAccessManager * ipSearcher = new QNetworkAccessManager(this);
	connect(ipSearcher,&QNetworkAccessManager::finished,this,[=](QNetworkReply * reply)
	{
		leServerAddress->setText(defaultServer/*reply->readAll()*/);
		leServerPort->setText(QString::number(defaultPort));
		reply->deleteLater();
		ipSearcher->deleteLater();
	});
	ipSearcher->get(QNetworkRequest({"http://api64.ipify.org/"}));

	serverLayout->addWidget(leServerAddress);
	serverLayout->addWidget(leServerPort);
	serverLayout->addWidget(btnStartServer);
	serverLayout->addWidget(serverStatus);

	//connect server signals
	connect(&Server::getInstance(), &Server::serverNotStarted, this, [=]() -> void
	{
		serverStatus->setText(tr("Server couldn't start. ")+QTime::currentTime().toString());
	});
	connect(&Server::getInstance(), &Server::serverStarted, this, [=] (QHostAddress address, int port) -> void
	{
		serverStatus->setText(tr("Server is running. ")+QTime::currentTime().toString()+"\n"+address.toString()+":"+QString::number(port));
		handleLeaderboardOnline();
	});
	connect(&Server::getInstance(), &Server::clientsChanged, this, [=] (int clientCount) -> void
	{
		serverStatus->setText(QString::number(clientCount) + tr(" client(s) connected to the server. ")+QTime::currentTime().toString());
	});
	connect(&Server::getInstance(), &Server::serverClosed, this, [&]()->void
	{
		serverStatus->setText(tr("Server stopped. ")+QTime::currentTime().toString());
	});

}

void StackWidget::handleLeaderboardOffline()
{
	leaderboard = new OfflineLeaderBoardWidget();
	stack->insertWidget(3, leaderboard);
	GameControll::getInstance().setLeaderboard(leaderboard);
	GameControll::addDefaultUsers();

	stack->setCurrentIndex(3);
}

void StackWidget::handleLeaderboardOnline()
{
	User* user = nullptr;
	if(leaderboard)
	{
		OnlineLeaderboardWidget* onlineLeaderboard = dynamic_cast<OnlineLeaderboardWidget*>(leaderboard);
		user = onlineLeaderboard->getLocalUser();
		onlineLeaderboard->deleteLater(); //TODO bleibt der User erhalten, auch wenn das Leaderboard glöscht wird?
	}
	leaderboard = new OnlineLeaderboardWidget();
	stack->insertWidget(3, leaderboard);
	GameControll::getInstance().setLeaderboard(leaderboard);

	OnlineLeaderboardWidget* onlineLeaderboard = dynamic_cast<OnlineLeaderboardWidget*>(leaderboard);
	if(onlineLeaderboard)
	{
		onlineLeaderboard->setLocalUser(GameControll::getInstance().initializeUser(user));
		onlineLeaderboard->initialize();
	}

	stack->setCurrentIndex(3);
}

