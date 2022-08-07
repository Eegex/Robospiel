#include "client.h"
#include "gamecontroll.h"
#include "offlineleaderboardwidget.h"
#include "onlineleaderboardwidget.h"
#include "server.h"
#include "stackwidget.h"

StackWidget::StackWidget(QWidget *parent) : QWidget{parent}
{
	QSizePolicy sp;
	sp.setHorizontalPolicy(QSizePolicy::Expanding);
	sp.setVerticalPolicy(QSizePolicy::Expanding);
	setSizePolicy(sp);

	btnSizePolicy.setHorizontalPolicy(QSizePolicy::Preferred);
	btnSizePolicy.setVerticalPolicy(QSizePolicy::Maximum);

	createNetworkSelection();
	createClientLayout();
	createServerLayout();

	stack = new QStackedLayout;
	QWidget* tmp = new QWidget(this);
	tmp->setLayout(networkSelection);
	stack->addWidget(tmp);
	tmp = new QWidget(this);
	tmp->setLayout(clientLayout);
	stack->addWidget(tmp);
	tmp = new QWidget(this);
	tmp->setLayout(serverLayout);
	stack->addWidget(tmp);

	backButton = new QPushButton(tr("Back"), this);
	backButton->setSizePolicy(btnSizePolicy);
	backButton->hide();

	layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
	layout->addLayout(stack);
	layout->addWidget(backButton,0,Qt::AlignHCenter);

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

QSize StackWidget::sizeHint() const
{
	return QSize(1000,1000);
}

void StackWidget::disconnectFromServer()
{
	if(leaderboard)
	{
		leaderboard->deleteLater();
		leaderboard = nullptr;
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

	btnClient = new QPushButton(tr("Join Server"),this);
	btnClient->setObjectName("btnClient");
	btnClient->setSizePolicy(btnSizePolicy);
	btnServer = new QPushButton(tr("Start Server"),this);
	btnServer->setObjectName("btnServer");
	btnServer->setSizePolicy(btnSizePolicy);
	btnOffline = new QPushButton(tr("Play offline"),this);
	btnOffline->setObjectName("btnOffline");
	btnOffline->setSizePolicy(btnSizePolicy);

	networkSelection = new QVBoxLayout(this);
	networkSelection->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
	networkSelection->addWidget(btnServer);
	networkSelection->addWidget(btnClient);
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
	leClientAddress = new QLineEdit(defaultServer,this);
	leClientAddress->setPlaceholderText(tr("Server Address"));
	leClientAddress->setSizePolicy(btnSizePolicy);
	leClientPort = new QLineEdit(QString::number(defaultPort),this);
	leClientPort->setSizePolicy(btnSizePolicy);
	leClientPort->setPlaceholderText(tr("Server Port"));
	leClientPort->setValidator(&intValidator);

	btnStartClient = new QPushButton(tr("Start Client"), this);
	btnStartClient->setSizePolicy(btnSizePolicy);
	connect(btnStartClient, &QPushButton::clicked, this, [=]()
	{
		Client::getInstance().startClient(leClientAddress->text(), leClientPort->text().toInt());
	});


	clientStatus = new QLabel(this);

	clientLayout = new QVBoxLayout(this);
	clientLayout->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
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
	leServerAddress = new QLineEdit(this);
	leServerAddress->setPlaceholderText(tr("Server Address"));
	leServerAddress->setSizePolicy(btnSizePolicy);
	leServerPort = new QLineEdit(this);
	leServerPort->setPlaceholderText(tr("Server Port"));
	leServerPort->setValidator(&intValidator);
	leServerPort->setSizePolicy(btnSizePolicy);

	btnStartServer = new QPushButton(tr("Start Server"), this);
	btnStartServer->setSizePolicy(btnSizePolicy);
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

	serverLayout = new QVBoxLayout(this);
	serverLayout->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
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
	if(leaderboard)
	{
		leaderboard->deleteLater();
		leaderboard = nullptr;
	}
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
		leaderboard = nullptr;
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
