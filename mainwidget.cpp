#include <QDebug>
#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	glMain = new QGridLayout(this);
	game = new GameControll(this);
	view = new BoardView(this);
	leaderboard = new LeaderBoardWidget(this);
	view->setBoard(game->createBoard(16, 16, 5));
	view->setMapping(game->getMapping());
	connect(view,&BoardView::action,game,&GameControll::triggerAction);
	connect(view,&BoardView::activePlayerChanged,game,&GameControll::activePlayerChanged);
	networkView = new NetworkView;
	settings = new SettingsDialog(*game->getMapping());
	lcd = new QLCDNumber(this);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setStyleSheet("QLCDNumber{"
					   "background-color: #000000;"
					   "color: #000000;"
					   "}");
	lcd->setMinimumSize(250,180);
    lcd->setDigitCount(2);
	glMain->addWidget(view,0,0,3,1,Qt::AlignCenter);
	glMain->addWidget(lcd,0,1,Qt::AlignCenter);
	glMain->addWidget(leaderboard,1,1,Qt::AlignCenter);
	connect(game,&GameControll::time,this,&MainWidget::updateTimer);
	adjustSize();
	connect(leaderboard->getUserCreationWidget(), &UserCreationWidget::userAdded, this, &MainWidget::addUser);

	connect(settings, &SettingsDialog::newMapping, game, &GameControll::setMapping);
}

void MainWidget::setMenuBar(QMenuBar * bar)
{
	menuBar = bar;
	aNewBoard = new QAction(tr("New Board"),this);
	//connect(aNewBoard,&QAction::triggered,board,&Board::newBoard);
	bar->addAction(aNewBoard);
	aNewTarget = new QAction(tr("New Target"),this);
	connect(aNewTarget,&QAction::triggered,game,&GameControll::nextTarget);
	bar->addAction(aNewTarget);
	aSettings = new QAction(tr("Settings"),this);
	connect(aSettings,&QAction::triggered,settings,&QDialog::exec);
	bar->addAction(aSettings);
	aNetworking = new QAction(tr("Networking"),this);
	connect(aNetworking,&QAction::triggered,networkView,&NetworkView::show);
	bar->addAction(aNetworking);
}

void MainWidget::addUser(struct UserData * newUser)
{
	qDebug()<<"addplayer in MainWidget";
	User *u = new User(newUser->name, newUser->colour, this);
	users.append(u);
	qDebug()<< u->getName();
	// adds new player in the frontend
	leaderboard->addPlayer(u);
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, &MainWidget::changeBidding);
}

void MainWidget::changeBidding(int bidding, QUuid id)
{
	qDebug()<<"change Bidding from "<<id.toString()<< "to" << bidding;
	for (User *u: users)
	{
		if (u->getId() == id)
		{
			u->setBidding(bidding);
			break;
		}
	}
}

void MainWidget::updateTimer(int remaining)
{
	QColor f(QColor::fromHsv(remaining*2,255,180));
	lcd->setStyleSheet("QLCDNumber{"
					   "background-color: #000000;"
					   "color: " + f.name() + ";"
					   "}");
	lcd->display(remaining);
}
