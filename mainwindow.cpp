#include "client.h"
#include "mainwindow.h"
#include "server.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	centralWidget = new MainWidget(this);
	setCentralWidget(centralWidget);
	centralWidget->setMenuBar(this->menuBar());
//    showMaximized();
	show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if(Server::isActive())
	{
		if(Server::connectedClientCount())
		{
			Server::switchServer();
			QTimer::singleShot(100,this,&MainWindow::close);
			event->ignore();
			return;
		}
		else
		{
			Server::closeServer();
		}
	}
	else if(Client::isActive())
	{
		Client::closeClient();
	}
	event->accept();
}
