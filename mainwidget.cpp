#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	glMain = new QGridLayout(this);
	game = new GameControll(this);
	view = new BoardView(this);
	view->setBoard(game->createBoard(16, 16, 5));
	view->setMapping(game->getMapping());
	networkView = new NetworkView;
	settings = new SettingsDialog;
	glMain->addWidget(view,0,0,Qt::AlignCenter);
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
