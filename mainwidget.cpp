#include <QDebug>
#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	glMain = new QGridLayout(this);
	game = new GameControll(this);
	view = new BoardView(this);
	leaderboard = new LeaderBoardWidget(this);
    view->setBoard(game->setBoard(new Board(16, 16, 5)));
	view->setMapping(game->getMapping());
	connect(view,&BoardView::action,game,&GameControll::triggerAction);
	connect(view,&BoardView::activePlayerChanged,game,&GameControll::activePlayerChanged);
    connect(view, &BoardView::lastAnimationAfterGoalHitEnded, this, [=](int moves)->void {
        QUuid currentPlayer = game->getActiveUserID();
        unsigned int activeUserIndex = leaderboard->getBiddingWidgetIndexByID(currentPlayer);
        game->nextTarget(); //Generate a new target, this should reset the current LeaderBoardWidget
        leaderboard->getUsers()->at(activeUserIndex)->incrementPoints(); //Increment the number of points of the player that's hit their goal
        qDebug()<<"User "<<leaderboard->getUsers()->at(activeUserIndex)->getName()<<" has successfully ended the round with "<<moves<<" moves, their current points are "<<leaderboard->getUsers()->at(activeUserIndex)->getPoints();
        currentMoves = -1; //This needs to be done because the actionTriggered will set it to 1 immediately after a goal is hit so the next player has 1 fewer move which would be bad
    });
	networkView = new NetworkView;
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
	connect(game, &GameControll::biddingDone, this, [&]()
	{
		leaderboard->sortByBidding();
        game->setActiveUserID(leaderboard->getUsers()->first()->getId());
		qDebug()<<"Bidding is done, Users are sorted, initial player is: "<<leaderboard->getUsers()->first()->getName()<<" with id "<<game->getActiveUserID();
	});
	connect(leaderboard->getUserOnlineWidget(), &UserOnlineWidget::userAdded, this, &MainWidget::addUser);
	connect(leaderboard->getUserOnlineWidget(), &UserOnlineWidget::biddingChangedOnline,this,&MainWidget::changeOnlyBidding);
	connect(game->getSettingsDialog(), &SettingsDialog::usernameChanged, leaderboard, &LeaderBoardWidget::setUsername);
	connect(game->getSettingsDialog(), &SettingsDialog::usercolorChanged, leaderboard, &LeaderBoardWidget::setUsercolor);
	connect(game, &GameControll::newOnlineUser, this, &MainWidget::addExistingUser);
    connect(game, &GameControll::actionTriggered, this, [&](PlayerAction action){
        if(action & PlayerAction::movement) //If player wants to move, increase the number of moves the player has done. This is being done to reset the field if the user has used all their available moves.
            currentMoves++;
        qDebug()<<"Current Moves are: "<<currentMoves;
        QUuid currentPlayer = game->getActiveUserID();
        unsigned int activeUserIndex = leaderboard->getBiddingWidgetIndexByID(currentPlayer);
        if(currentMoves >= leaderboard->getUsers()->at(activeUserIndex)->getBidding()){ //Needs to be GEQ because otherwise the player could make an extra move as the "reached goal" signal is overriding the failure
            qDebug()<<"User couldn't end the round in the specified bid of "<< leaderboard->getUsers()->at(activeUserIndex)->getBidding()<<", the next user is being drawn";
            currentMoves = 0; //Reset Current Moves
            game->getBoard()->resetMoves();
            if(leaderboard->getNumOfUsers() > 1 && activeUserIndex < leaderboard->getNumOfUsers()-1){ //Not at last player yet, noch haben nicht alle versagt
                UserBiddingWidget* user = leaderboard->getUsers()->at(++activeUserIndex); //Liste ist bereits sortiert (siehe oben), daher ist der nächste User in der Liste der User mit dem nächsthöheren Bidding
                game->setActiveUserID(user->getId()); //Setze nächsten Spieler als aktiv
                game->getBoard()->revertToBeginning(); //Setze Spielerpositionen zurück
                qDebug()<<"Active User is now "<<user->getName();
            }else{ //Alles Versager
                qDebug()<<"No User could end the round in their specified bid.";
                game->nextTarget(); //TODO wait for end of animation
            }
        }
    });
}

void MainWidget::setMenuBar(QMenuBar * bar)
{
	menuBar = bar;
	mNewBoard = new QMenu(tr("New Board"),this);
	waHeight = new QWidgetAction(this);
	sbHeight = new QSpinBox(this);
	waWidth = new QWidgetAction(this);
	sbWidth = new QSpinBox(this);
	waPlayer = new QWidgetAction(this);
	sbPlayer = new QSpinBox(this);
	aNewBoard = new QAction(tr("Create Board"),this);
	sbHeight->setMinimum(5);
	sbWidth->setMinimum(5);
	sbPlayer->setMinimum(1);
	waHeight->setDefaultWidget(sbHeight);
	waWidth->setDefaultWidget(sbWidth);
	waPlayer->setDefaultWidget(sbPlayer);
	mNewBoard->addAction(new QAction(tr("Height:")));
	mNewBoard->addAction(waHeight);
	mNewBoard->addAction(new QAction(tr("Width:")));
	mNewBoard->addAction(waWidth);
	mNewBoard->addAction(new QAction(tr("Player count:")));
	mNewBoard->addAction(waPlayer);
	mNewBoard->addAction(aNewBoard);
	connect(aNewBoard,&QAction::triggered,this,&MainWidget::createBoard);
	bar->addMenu(mNewBoard);
	aEditBoard = new QAction(tr("Edit Board"),this);
	connect(aEditBoard,&QAction::triggered,this,&MainWidget::editBoard);
	bar->addAction(aEditBoard);
	aNewTarget = new QAction(tr("New Target"),this);
	connect(aNewTarget,&QAction::triggered,game,&GameControll::nextTarget);
	bar->addAction(aNewTarget);
	aSettings = new QAction(tr("Settings"),this);
	connect(aSettings,&QAction::triggered,game,&GameControll::showSettings);
	bar->addAction(aSettings);
}

//this method is only for offline-users
void MainWidget::addUser(struct UserData * newUser)
{
	qDebug()<<"adduser in MainWidget";
	User *u = new User(newUser->name, newUser->colour, this);
	users.append(u);
	qDebug()<< u->getName();
	// adds new user in the frontend
	leaderboard->addUser(u);
    connect(game, &GameControll::newRound, leaderboard->getUsers()->last(), &UserBiddingWidget::resetBidding);
	connect(game, &GameControll::biddingDone, leaderboard->getUsers()->last(), &UserBiddingWidget::deactivateBtn);
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, &MainWidget::changeBidding);
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingReset, this, &MainWidget::changeBidding);
	/*connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingReset, this, [&](const int, const QUuid){
		leaderboard->updateLayout();
	});*/
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, [&](const int userBidding, const QUuid id){ //Connect the biddingChanged Signal to triggerAction with appropriate argument{
		game->triggerAction(PlayerAction::sendBidding, id);
	});
}

void MainWidget::addExistingUser(User* user)
{
    for(User* u: users)
        if(u->getId()==user->getId())
            return;
    users.append(user);
    game->triggerAction(PlayerAction::newUser, user->getId());
}

void MainWidget::createBoard()
{
	if(view)
	{
        view->setBoard(game->setBoard(new Board(sbWidth->value(),sbHeight->value(),sbPlayer->value())));
		glMain->addWidget(view,0,0,3,1,Qt::AlignCenter); //alignment or size dies without this line ¯\_(ツ)_/¯
	}
	else
	{
        edit->setBoard(game->setBoard(new Board(sbWidth->value(),sbHeight->value(),sbPlayer->value())));
		glMain->addWidget(edit,0,0,3,1,Qt::AlignCenter); //same here...
	}
}

void MainWidget::editBoard()
{
	if(game->getCurrentPhase() == GameControll::Phase::idle && !edit)
	{
		edit = new BoardEditor(this);
		edit->setBoard(game->getBoard());
		glMain->addWidget(edit,0,0,3,1,Qt::AlignCenter);
		aEditBoard->setText(tr("Stop editing"));
		delete view;
		view = nullptr;
	}
	else
	{
		view = new BoardView(this);
		view->setBoard(game->getBoard());
		view->setMapping(game->getMapping());
		connect(view,&BoardView::action,game,&GameControll::triggerAction);
		connect(view,&BoardView::activePlayerChanged,game,&GameControll::activePlayerChanged);
		glMain->addWidget(view,0,0,3,1,Qt::AlignCenter);
		aEditBoard->setText(tr("Edit Board"));
		delete edit;
		edit = nullptr;
	}
}

void MainWidget::changeBidding(int bidding, QUuid id)
{
	qDebug()<<"Called Function Change Bidding from "<<id.toString()<< "to" << bidding;
	for (User *u: users)
	{
		if (u->getId() == id)
		{
			u->setBidding(bidding);
			break;
		}
	}
}

void MainWidget::changeOnlyBidding(int bidding)
{
	qDebug()<<"changeOnly Bidding to"<<bidding;
	users.at(0)->setBidding(bidding);
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
