#include <QDebug>
#include "mainwidget.h"
#include "onlineleaderboardwidget.h"
#include "server.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	GameControll::initializeConnections();
	glMain = new QGridLayout(this);
	//GameControll::setLeaderboard(userView->getLeaderboard());


	aServerSwitch = new QAction("Switch server",this);


	aServerSwitch->setVisible(false);
	aServerSwitch->setEnabled(false); // should only be enabled as soon as we have the leaderboard and we can actually start the game
	connect(&Server::getInstance(),&Server::clientsChanged,this,[&](int count)
	{
		enableServerSwitchBtn(count);
	});

	initializeView(GameControll::setBoard(new Board(16, 16, 5)), GameControll::getMapping());
	//connect(view, &BoardView::lastAnimationAfterGoalHitEnded, game, &GameControll::calculateWinner);
	lcd = new QLCDNumber(this);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setStyleSheet("QLCDNumber{"
					   "background-color: #000000;"
					   "color: #DD0000;"
					   "}");
	lcd->setMinimumSize(250,180);
	lcd->setDigitCount(2);
	dlGuide->setMaximumHeight(30);
	glMain->addWidget(dlGuide,0,0,1,2,Qt::AlignHCenter);
	glMain->addWidget(view,1,0,4,1,Qt::AlignCenter);

	glMain->addWidget(lcd,1,1,Qt::AlignCenter);
	connect(&GameControll::getInstance(),&GameControll::updateGuide,this,&MainWidget::updateGuide);
	connect(&GameControll::getInstance(), &GameControll::newBoard, this, [=](Board * newBoard)
	{
		initializeView(newBoard, GameControll::getMapping());
	});
	connect(&GameControll::getInstance(),&GameControll::focusBoard,this,&MainWidget::focusBoard);
	adjustSize();


	connect(aServerSwitch, &QAction::triggered, this, &MainWidget::handleServerSwitch);
	connect(&GameControll::getInstance(), &GameControll::setBoardEnabled, this, [&](bool enabled)
	{
		if(edit)
		{
			edit->setEnabled(enabled);
		}
		else
		{
			view->setEnabled(enabled);
		}
	});

	stack = new StackWidget(this);
	glMain->addWidget(stack,4,1,Qt::AlignCenter);
	connect(&GameControll::getInstance(),&GameControll::time,this,&MainWidget::updateTimer);

}


void MainWidget::handleServerSwitch()
{
	GameControll::initiateServerSwitch();
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
	handleKeyPress(event->key());
	QWidget::keyPressEvent(event);
}

void MainWidget::handleKeyPress(int key)
{
	for(const KeyMapping * k:qAsConst(*GameControll::getMapping()))
	{
		if(*k == key)
		{
			if(k->getAction() == PlayerAction::vote)
			{
				OnlineLeaderboardWidget* leaderboard = dynamic_cast<OnlineLeaderboardWidget*>(GameControll::getLeaderboard());
				if(leaderboard)
				{
					leaderboard->handleActionButtonRelease();
				}

			}
			else
			{
				GameControll::triggerAction(k->getAction());
			}
			return;
		}
	}
}





void MainWidget::setMenuBar(QMenuBar * bar)
{
	menuBar = bar;
	mNewGame = new QMenu(tr("Board"),this);
	waHeight = new QWidgetAction(this);
	sbHeight = new QSpinBox(this);
	waWidth = new QWidgetAction(this);
	sbWidth = new QSpinBox(this);
	waPlayer = new QWidgetAction(this);
	sbPlayer = new QSpinBox(this);
	aNewBoard = new QAction(tr("Create Board"),this);
	aGoToIdle = new QAction(tr("Idle"),this);
	aGoToIdle->setDisabled(true);
	connect(aNewBoard,&QAction::triggered,this,&MainWidget::createBoard);
	sbHeight->setMinimum(5);
	sbHeight->setValue(view->getBoard()->getSize().height());
	sbWidth->setMinimum(5);
	sbWidth->setValue(view->getBoard()->getSize().width());
	sbPlayer->setMinimum(1);
	sbPlayer->setMaximum((sbWidth->value()*sbHeight->value())-1);
	sbPlayer->setValue(view->getBoard()->players.length());
	waHeight->setDefaultWidget(sbHeight);
	waWidth->setDefaultWidget(sbWidth);
	waPlayer->setDefaultWidget(sbPlayer);
	mNewGame->addAction(new QAction(tr("Height:")));
	mNewGame->actions().last()->setDisabled(true);
	mNewGame->addAction(waHeight);
	mNewGame->addAction(new QAction(tr("Width:")));
	mNewGame->actions().last()->setDisabled(true);
	mNewGame->addAction(waWidth);
	mNewGame->addAction(new QAction(tr("Player count:")));
	mNewGame->actions().last()->setDisabled(true);
	mNewGame->addAction(waPlayer);
	mNewGame->addAction(aNewBoard);

	//TODO: Wenn nicht gebraucht ausgrauen...

	mNewStuff =  new QMenu(tr("New"),this);
	aNewWalls = new QAction(tr("Walls"),this);
	aNewSeeker = new QAction(tr("Seeker"),this);
	aNewPlayers = new QAction(tr("Players"),this);
	aNewAll = new QAction(tr("All"),this);
	aNewTarget = new QAction(tr("Target"),this);

	mNewStuff->addAction(aNewAll);
	mNewStuff->addAction(aNewWalls);
	mNewStuff->addAction(aNewPlayers);
	mNewStuff->addAction(aNewTarget);
	mNewStuff->addAction(aNewSeeker);
	mNewStuff->addMenu(mNewGame);

#ifdef __APPLE__
	QMenu *menu1 = new QMenu(tr("Edit Board"),this);
	aEditBoard = new QAction(tr("Edit Board"),this);
	menu1->addAction(aEditBoard);
	QMenu *menu2 = new QMenu(tr("Next Target"),this);
	aNextTarget = new QAction(tr("Next Target"),this);
	menu2->addAction(aNextTarget);

	QMenu *menu4 = new QMenu(tr("Reset points"),this);
	aResetPoints = new QAction(tr("Reset points"),this);
	menu4->addAction(aResetPoints);

	QMenu *menu3 = new QMenu(tr("Settings"),this);
	aSettings = new QAction(tr("Settings"),this);
	menu3->addAction(aSettings);
	menu3->addAction(aGoToIdle);

	QMenu *menu5 = new QMenu(tr("Debug"),this);
	aDebugger = new QAction(tr("Debug"),this);
	menu5->addAction(aDebugger);
	QMenu *menu6 = new QMenu(tr("Server Switch"),this);
	menu6->addAction(aServerSwitch);

	bar->addMenu(mNewStuff);
	bar->addMenu(menu1);
	bar->addMenu(menu2);
	bar->addMenu(menu3);
	bar->addMenu(menu4);
	bar->addMenu(menu5);
	bar->addMenu(menu6);
#else
	bar->addMenu(mNewStuff);
	aEditBoard = new QAction(tr("Edit Board"),this);
	bar->addAction(aEditBoard);

	aNextTarget = new QAction(tr("Next Target"),this);
	//bar->addAction(aNextTarget);

	aResetPoints = new QAction(tr("Reset points"),this);
//	bar->addAction(aResetPoints);

	aSettings = new QAction(tr("Settings"),this);
	bar->addAction(aSettings);
	bar->addAction(aGoToIdle);
	aDebugger = new QAction(tr("Debug"),this);
	bar->addAction(aDebugger);
	bar->addAction(aServerSwitch);


#endif
	connect(aDebugger,&QAction::triggered,&GameControll::getInstance(),&GameControll::startNetworkDebugger);
	connect(aDebugger,&QAction::triggered,this,[&]()
	{
		aDebugger->setText("Clear Log");
	});
	connect(aSettings,&QAction::triggered,&GameControll::getInstance(),&GameControll::showSettings);
	connect(aResetPoints, &QAction::triggered, this, [=]()->void
	{
		GameControll::triggerAction(PlayerAction::resetPoints);
	});
	connect(aEditBoard,&QAction::triggered,this,[=]()->void
	{
		if(edit)
		{
			GameControll::triggerActionWithData(PlayerAction::editBoard,{{"board", GameControll::getBoard()->toBinary()}});
			GameControll::triggerActionWithData(PlayerAction::syncRandomGenerators,{{"Seed",QTime::currentTime().msecsSinceStartOfDay()}});
		}
		else
		{
			GameControll::triggerAction(PlayerAction::blockBoard);
		}
		this->editBoard();
	});
	connect(aNextTarget,&QAction::triggered,this, [=]()->void
	{
		GameControll::triggerAction(PlayerAction::nextTarget);
	});
	connect(aGoToIdle, &QAction::triggered, &GameControll::getInstance(), [=]()->void
	{
		GameControll::triggerAction(PlayerAction::setIdle);
	});
	connect(&GameControll::getInstance(), &GameControll::enableMenus, this, &MainWidget::enableMenus);
	connect(&GameControll::getInstance(), &GameControll::enableServerSwitchBtn, this, &MainWidget::enableServerSwitchBtn);
	connect(&GameControll::getInstance(), &GameControll::enableIdleBtn, this, &MainWidget::enableIdle);
	connect(sbWidth, SIGNAL(valueChanged(int)), this, SLOT(updatePlayerMaximum(int)));
	connect(sbHeight, SIGNAL(valueChanged(int)), this, SLOT(updatePlayerMaximum(int)));
	connectView(view);
}

void MainWidget::updatePlayerMaximum(int)
{
	sbPlayer->setMaximum(sbWidth->value()*sbHeight->value()-1);
}

void MainWidget::connectView(BoardView * view)
{
	if(!aNewWalls)
	{
		return;
	}
	connect(aNewWalls,&QAction::triggered,view,&BoardView::makeNewWalls);
	connect(aNewSeeker,&QAction::triggered,view,&BoardView::makeNewSeeker);
	connect(aNewPlayers,&QAction::triggered,view,&BoardView::makeNewPlayers);
	connect(aNewAll,&QAction::triggered,view,&BoardView::makeNewAll);
	connect(aNewTarget,&QAction::triggered,view,&BoardView::makeNewTarget);
}

void MainWidget::createBoard()
{
	if(view)
	{
		view->setBoard(GameControll::setBoard(new Board(sbWidth->value(),sbHeight->value(),sbPlayer->value())));
		glMain->addWidget(view,1,0,5,1,Qt::AlignCenter); //alignment or size dies without this line ¯\_(ツ)_/¯
	}
	else
	{
		edit->setBoard(GameControll::setBoard(new Board(sbWidth->value(),sbHeight->value(),sbPlayer->value())));
		glMain->addWidget(edit,1,0,5,1,Qt::AlignCenter); //same here...
	}
	GameControll::triggerActionWithData(PlayerAction::editBoard,{{"board",GameControll::getBoard()->toBinary()}});
}

void MainWidget::updateGuide(const QString & txt)
{
	dlGuide->setText(txt);
}

void MainWidget::enableMenus(bool boolean)
{
	mNewStuff->setEnabled(boolean);
	aEditBoard->setEnabled(boolean);
}

void MainWidget::enableBoard(bool enabled)
{
	if(view)
	{
		view->setEnabled(enabled);
	}
	else
	{
		edit->setEnabled(enabled);
	}
}

void MainWidget::enableIdle(bool boolean){
	aGoToIdle->setEnabled(boolean);
}

void MainWidget::focusBoard()
{
	if(view)
	{
		view->setFocus();
	}
}



void MainWidget::enableServerSwitchBtn(bool boolean)
{
	aServerSwitch->setEnabled(boolean);
	aServerSwitch->setVisible(boolean);
}

void MainWidget::editBoard()
{
	bool enabled = false;
	if(edit)
	{
		enabled = edit->isEnabled();
	}
	else
	{
		enabled = view->isEnabled();
	}
	if(GameControll::getCurrentPhase() == GameControll::Phase::idle && !edit)
	{
		edit = new BoardEditor(this);
		edit->setBoard(GameControll::getBoard());
		connectView(edit);
		glMain->addWidget(edit,1,0,4,1,Qt::AlignCenter);
		aEditBoard->setText(tr("Stop editing"));
		delete view;
		view = nullptr;
	}
	else
	{
		initializeView(GameControll::getBoard(), GameControll::getMapping());
		aEditBoard->setText(tr("Edit Board"));
		delete edit;
		edit = nullptr;
	}
	if(!edit)
	{
		view->setEnabled(enabled);
	}
}

void MainWidget::updateTimer(int remaining)
{
	QColor f(QColor::fromHsv(remaining * 2,255,180));
	const User * top = GameControll::getMinBid();
	if(top && GameControll::showTopBidding())
	{
		f = top->getColor();
	}
	if(f.value() < 100)
	{
		lcd->setStyleSheet("QLCDNumber{"
						   "background-color: #FFFFFF;"
						   "color: " + f.name() + ";"
												  "}");
	}
	else
	{
		lcd->setStyleSheet("QLCDNumber{"
						   "background-color: #000000;"
						   "color: " + f.name() + ";"
												  "}");
	}
	lcd->display(remaining);
}

void MainWidget::initializeView(Board* b, QVector<KeyMapping*>* m)
{
	if(view)
	{
		view->deleteLater();
	}
	view = new BoardView(this);
	view->setBoard(b);
	view->setMapping(m);
	view->setFocusPolicy(Qt::ClickFocus);
	connectView(view);
	connect(view, &BoardView::animationEnded, &GameControll::getInstance(), [=]()->void{
		if(GameControll::getActionWhenAnimationEnded())
		{
			(GameControll::getInstance().*GameControll::getActionWhenAnimationEnded())();
			GameControll::setActionWhenAnimationEnded(nullptr);
		}
	});
	connect(view,&BoardView::action,&GameControll::getInstance(),&GameControll::triggerAction);
	connect(view,&BoardView::activePlayerChanged,&GameControll::getInstance(),[=](int playerNumber)->void
	{
		if(GameControll::getInstance().localUserIsActiveUser())
		{
			GameControll::triggerActionWithData(PlayerAction::playerSwitch, {{"playerNumber", playerNumber}});
		}

	});
	glMain->addWidget(view,1,0,4,1,Qt::AlignCenter);
}
