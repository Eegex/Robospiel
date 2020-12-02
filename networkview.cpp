#include "networkview.h"
#include "server.h"
#include "client.h"
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>

NetworkView::NetworkView(QWidget *parent) : QWidget(parent)
{
    QValidator *intValidator = new QIntValidator(this);

    layout = new QGridLayout(this);
    choiceGroup = new QButtonGroup(this);
    btnClient = new QRadioButton(tr("Join Server"), this);
    btnServer = new QRadioButton(tr("Host Server"), this);
    btnClient->setChecked(true);
    choiceGroup->addButton(btnClient);
    choiceGroup->addButton(btnServer);

    //Client gui
    clientContainer = new QWidget(this);
    gridClient=new QGridLayout(clientContainer);
    leClientAddress = new QLineEdit(clientContainer);
    leClientAddress->setPlaceholderText(tr("Client Address"));
    leClientPort = new QLineEdit(clientContainer);
    leClientPort->setPlaceholderText(tr("Client Port"));
    leClientPort->setValidator(intValidator);

    QPushButton* btnStartClient = new QPushButton(tr("Start Client"), clientContainer);
    connect(btnStartClient, &QPushButton::clicked, this, &NetworkView::addClient);
    QPushButton* btnStopClient = new QPushButton(tr("Stop Client"), clientContainer);
    connect(btnStopClient, &QPushButton::clicked, this, [&]()->void {Client::getInstance().closeClient();});
    clientStatus = new QLabel(clientContainer);

    //only for testing
    leMessageToServer = new QLineEdit(tr("Message to server"), clientContainer);
    QPushButton* btnSendToServer = new QPushButton(tr("Send Message to server"), clientContainer);
    connect(btnSendToServer, &QPushButton::clicked, this, &NetworkView::sendToServer);

    gridClient->addWidget(leClientAddress);
    gridClient->addWidget(leClientPort);
    gridClient->addWidget(btnStartClient);
    gridClient->addWidget(btnStopClient);
    gridClient->addWidget(clientStatus);
    gridClient->addWidget(leMessageToServer);
    gridClient->addWidget(btnSendToServer);

    clientContainer->setLayout(gridClient);

    //Server gui
    serverContainer = new QWidget(this);
    gridServer=new QGridLayout(serverContainer);
    leServerAddress = new QLineEdit(serverContainer);
    leServerAddress->setPlaceholderText(tr("Server Address"));
    leServerPort = new QLineEdit(serverContainer);
    leServerPort->setPlaceholderText(tr("Server Port"));
    leServerPort->setValidator(intValidator);

    QPushButton* btnStartServer = new QPushButton(tr("Start Server"), serverContainer);
    connect(btnStartServer, &QPushButton::clicked, this, &NetworkView::addServer);
    QPushButton* btnStopServer = new QPushButton(tr("Stop Server"), serverContainer);
    connect(btnStopServer, &QPushButton::clicked, this, [&]()->void
            {
                Server::getInstance().closeServer();
                serverStatus->setText(tr("Server stopped. ")+QTime::currentTime().toString());
            });
    serverStatus = new QLabel(serverContainer);

    //only for testing
    leMessageToClients = new QLineEdit(tr("Message to clients"), serverContainer);
    QPushButton* btnSendToClients = new QPushButton(tr("Send Message to all clients"), serverContainer);
    connect(btnSendToClients, &QPushButton::clicked, this, &NetworkView::sendToClients);

    gridServer->addWidget(leServerAddress);
    gridServer->addWidget(leServerPort);
    gridServer->addWidget(btnStartServer);
    gridServer->addWidget(btnStopServer);
    gridServer->addWidget(serverStatus);
    gridServer->addWidget(leMessageToClients);
    gridServer->addWidget(btnSendToClients);

    serverContainer->setLayout(gridServer);


    layout->addWidget(btnClient);
    layout->addWidget(clientContainer);
    layout->addWidget(btnServer);
    layout->addWidget(serverContainer);

    setLayout(layout);
    setWindowTitle(tr("Client/Server Settings"));
    
    connect(&Server::getInstance(), &Server::serverNotStarted, this, [=]() -> void
    {
        serverStatus->setText(tr("Server couldn't start. ")+QTime::currentTime().toString());
    });
    connect(&Server::getInstance(), &Server::serverStarted, this, [=] (QHostAddress address, int port) -> void
    {
        serverStatus->setText(tr("Server is running. ")+QTime::currentTime().toString()+"\n"+address.toString()+":"+QString::number(port));
    });
    connect(&Server::getInstance(), &Server::clientsChanged, this, [=] (int clientCount) -> void
    {
        serverStatus->setText(QString::number(clientCount) + tr(" client(s) connected to the server. ")+QTime::currentTime().toString());
    });

    connect(&Client::getInstance(), &Client::errorInClient, this, [&](QAbstractSocket::SocketError socketError)->void
    {
        clientStatus->setText(tr("Error in client: ")+QString::number(socketError)+" "+QTime::currentTime().toString());
    });
    connect(&Client::getInstance(), &Client::clientStarted, this, [&]()->void
    {
        clientStatus->setText(tr("Client started. ")+QTime::currentTime().toString());
    });
    connect(&Client::getInstance(), &Client::clientClosed, this, [&]()->void
    {
        clientStatus->setText(tr("Client stopped. ")+QTime::currentTime().toString());
    });

}

void NetworkView::addServer()
{
    Server::getInstance().startServer(leServerAddress->text(), leServerPort->text().toInt());
}
void NetworkView::sendToClients()
{
    int errorCount=Server::getInstance().sendMessageToClients(leMessageToClients->text());
    if(errorCount!=0)
    {
        serverStatus->setText(tr("Message was not sent to ")+QString::number(errorCount)+tr(" clients. ")+QTime::currentTime().toString());
    }
}

void NetworkView::addClient()
{
    Client::getInstance().startClient(leServerAddress->text(), leServerPort->text().toInt());
}

void NetworkView::sendToServer()
{
    bool okay = Client::getInstance().sendMessageToServer(leMessageToServer->text());
    if(!okay)
    {
        clientStatus->setText(tr("Message was not sent to the server. ")+QTime::currentTime().toString());
    }
}


