#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	glMain = new QGridLayout(this);
	board = new Board(16, 16, 5,this);
	view = new BoardView(this);
	view->setBoard(board);
	networkView = new NetworkView;
	glMain->addWidget(view,0,0,Qt::AlignCenter);
	adjustSize();
}

void MainWidget::setMenuBar(QMenuBar * bar)
{
	menuBar = bar;
	aNetworking = new QAction(tr("Networking"),this);
    connect(aNetworking,&QAction::triggered,networkView,&NetworkView::show);
	bar->addAction(aNetworking);
	aNewBoard = new QAction(tr("New Board"),this);
	//connect(aNewBoard,&QAction::triggered,board,&Board::newBoard);
	bar->addAction(aNewBoard);
	aNewTarget = new QAction(tr("New Target"),this);
	connect(aNewTarget,&QAction::triggered,board,&Board::startNewRound);
	bar->addAction(aNewTarget);
}
