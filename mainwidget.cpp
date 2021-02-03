#include <QDebug>
#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    GameControll::initializeConnections();
	glMain = new QGridLayout(this);
    leaderboard = new LeaderBoardWidget(this);

    initializeView(GameControll::setBoard(new Board(16, 16, 5)), GameControll::getMapping());


    //connect(view, &BoardView::lastAnimationAfterGoalHitEnded, game, &GameControll::calculateWinner);
	networkView = new NetworkView;
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
	glMain->addWidget(lcd,1,1,Qt::AlignCenter);
	glMain->addWidget(leaderboard,2,1,2,1,Qt::AlignCenter);
    connect(&GameControll::getInstance(),&GameControll::time,this,&MainWidget::updateTimer);
    connect(&GameControll::getInstance(),&GameControll::updateGuide,this,&MainWidget::updateGuide);
	adjustSize();
    GameControll::setLeaderboard(leaderboard);
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
	connect(aNewBoard,&QAction::triggered,this,&MainWidget::createBoard);
	sbHeight->setMinimum(5);
	sbHeight->setValue(view->getBoard()->getSize().height());
	sbWidth->setMinimum(5);
	sbWidth->setValue(view->getBoard()->getSize().width());
	sbPlayer->setMinimum(1);
	sbPlayer->setMaximum(sbWidth->value()*sbHeight->value()-1);
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
	connect(aNewWalls,&QAction::triggered,view,&BoardView::makeNewWalls);
	aNewSeeker = new QAction(tr("Seeker"),this);
	connect(aNewSeeker,&QAction::triggered,view,&BoardView::makeNewSeeker);
	aNewPlayers = new QAction(tr("Players"),this);
	connect(aNewPlayers,&QAction::triggered,view,&BoardView::makeNewPlayers);
	aNewAll = new QAction(tr("All"),this);
	connect(aNewAll,&QAction::triggered,view,&BoardView::makeNewAll);
	aNewTarget = new QAction(tr("Target"),this);
	connect(aNewTarget,&QAction::triggered,view,&BoardView::makeNewTarget);

	mNewStuff->addAction(aNewAll);
	mNewStuff->addAction(aNewWalls);
	mNewStuff->addAction(aNewPlayers);
	mNewStuff->addAction(aNewTarget);
	mNewStuff->addAction(aNewSeeker);
	mNewStuff->addMenu(mNewGame);

    #ifdef __APPLE__
    QMenu *menu1 = new QMenu(tr("Edit Board"),this);
    aEditBoard = new QAction(tr("Edit Board"),this);
    connect(aEditBoard,&QAction::triggered,this,&MainWidget::editBoard);
    menu1->addAction(aEditBoard);
    QMenu *menu2 = new QMenu(tr("Next Target"),this);
    aNextTarget = new QAction(tr("Next Target"),this);
    connect(aNextTarget,&QAction::triggered,game,&GameControll::nextTarget);
    menu2->addAction(aNextTarget);
    QMenu *menu3 = new QMenu(tr("Settings"),this);
    aSettings = new QAction(tr("Settings"),this);
    connect(aSettings,&QAction::triggered,game,&GameControll::showSettings);
    menu3->addAction(aSettings);
    bar->addMenu(mNewStuff);
    bar->addMenu(menu1);
    bar->addMenu(menu2);
    bar->addMenu(menu3);
    #else
    bar->addMenu(mNewStuff);
	aEditBoard = new QAction(tr("Edit Board"),this);
	connect(aEditBoard,&QAction::triggered,this,&MainWidget::editBoard);
    bar->addAction(aEditBoard);
	aNextTarget = new QAction(tr("Next Target"),this);
    connect(aNextTarget,&QAction::triggered,&GameControll::getInstance(),&GameControll::nextTarget);
    bar->addAction(aNextTarget);
	aSettings = new QAction(tr("Settings"),this);
    connect(aSettings,&QAction::triggered,&GameControll::getInstance(),&GameControll::showSettings);
    bar->addAction(aSettings);
    #endif

}


void MainWidget::createBoard()
{
	if(view)
	{
        view->setBoard(GameControll::setBoard(new Board(sbWidth->value(),sbHeight->value(),sbPlayer->value())));
		glMain->addWidget(view,1,0,3,1,Qt::AlignCenter); //alignment or size dies without this line ¯\_(ツ)_/¯
	}
	else
	{
        edit->setBoard(GameControll::setBoard(new Board(sbWidth->value(),sbHeight->value(),sbPlayer->value())));
		glMain->addWidget(edit,1,0,3,1,Qt::AlignCenter); //same here...
	}
}

void MainWidget::updateGuide(const QString & txt)
{
	dlGuide->setText(txt);
}

void MainWidget::editBoard()
{
    if(GameControll::getCurrentPhase() == GameControll::Phase::idle && !edit)
	{
		edit = new BoardEditor(this);
        edit->setBoard(GameControll::getBoard());
		glMain->addWidget(edit,1,0,3,1,Qt::AlignCenter);
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
}

void MainWidget::updateTimer(int remaining)
{
	QColor f(QColor::fromHsv(remaining*2,255,180));
    User * top = GameControll::getMinBid();
    if(top && GameControll::showTopBidding())
	{
		f = top->getColor();
	}
	lcd->setStyleSheet("QLCDNumber{"
					   "background-color: #000000;"
					   "color: " + f.name() + ";"
					   "}");
	lcd->display(remaining);
}

void MainWidget::initializeView(Board* b, QVector<KeyMapping*>* m)
{
    view = new BoardView(this);
    view->setBoard(b);
    view->setMapping(m);
    connect(view,&BoardView::action,&GameControll::getInstance(),&GameControll::triggerAction);
    connect(view,&BoardView::activePlayerChanged,&GameControll::getInstance(),[=](int playerNumber)->void
    {
        QJsonObject data;
        data.insert("playerNumber", playerNumber);
        GameControll::triggerActionsWithData(PlayerAction::playerSwitch, data);
    });
    glMain->addWidget(view,1,0,3,1,Qt::AlignCenter);
}
