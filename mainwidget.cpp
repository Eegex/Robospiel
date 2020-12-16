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
	networkView = new NetworkView;
	settings = new SettingsDialog;
	lcd = new QLCDNumber(this);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setStyleSheet("QLCDNumber{"
					   "background-color: #000000;"
					   "color: #000000;"
					   "}");
	lcd->setMinimumSize(250,180);
	lcd->setDigitCount(2);
	glMain->addWidget(view,0,0,Qt::AlignCenter);
	glMain->addWidget(lcd,0,1,Qt::AlignCenter);
	glMain->addWidget(leaderboard,1,1,Qt::AlignCenter);
	connect(game,&GameControll::time,this,&MainWidget::updateTimer);
	adjustSize();
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

void MainWidget::updateTimer(int remaining)
{
	QColor f(QColor::fromHsv(remaining*2,255,180));
	lcd->setStyleSheet("QLCDNumber{"
					   "background-color: #000000;"
					   "color: " + f.name() + ";"
					   "}");
	lcd->display(remaining);
}
