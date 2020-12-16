#include <QDebug>
#include "LeaderBoardWidget.h"
#include "PlayerBiddingWidget.h"
#include "PlayerCreationWidget.h"
#include "user.h"

LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
    playerCreationWidget->hide();
    addBtn->setText("Add new Player");
    for(unsigned int i = 0; i<numOfPlayers; i++)
        lay->addWidget(players.at(i), i, 0);
    lay->addWidget(addBtn, numOfPlayers, 0);
    for(unsigned int i = 0; i<numOfPlayers; i++)
    {
        players.at(i)->accept->setEnabled(i == currentPlayer?true:false);
        players.at(i)->lSpinBox->setEnabled(i == currentPlayer?true:false);
    }
    setLayout(lay);
    connect(addBtn,&QPushButton::clicked,this,&LeaderBoardWidget::newPlayer);
    connect(playerCreationWidget, &PlayerCreationWidget::playerAdded, this, &LeaderBoardWidget::addPlayer);
    //static_cast<PlayerBiddingWidget*>(calloc(5, sizeof(PlayerBiddingWidget*)));
    /*players[0] = new PlayerBiddingWidget(this);
    players.at(0)->setName("TestName");
    players.at(0)->setBidding(10);
    lay->addWidget(players.at(0));*/
}

void LeaderBoardWidget::sortByBidding()
{ //Sort by Bidding, Person with the lowest bid at the top, might not be a good idea
    PlayerBiddingWidget * sortedPlayers;
    sortedPlayers = (PlayerBiddingWidget*) calloc(numOfPlayers, sizeof(PlayerBiddingWidget*));
    unsigned int minBid = players.at(0)->getBidding();
    for(unsigned int i = 0; i<numOfPlayers-1; i++)
    {
        //Do Stuff, this has to be done better
        //lay->addWidget(lay->takeAt(0), i, 0);
    }
    free(sortedPlayers);
}

void LeaderBoardWidget::addPlayer(User * newUser)
{
    qDebug()<<"addPlayer in LeaderBoardWidget";
    qDebug()<<"Name: "+newUser->getName();
    PlayerBiddingWidget * newWidget = new PlayerBiddingWidget(this);
    newWidget->setName(newUser->getName());
    players.append(newWidget);
    //players[numOfPlayers] = player;
    lay->addWidget(players.at(numOfPlayers));

    for(unsigned int i = 0; i<numOfPlayers; i++)
        lay->addWidget(players.at(i), i, 0);
    lay->addWidget(addBtn, numOfPlayers, 0); //New Player Button under all Players
    for(unsigned int i = 0; i<numOfPlayers; i++)
    {
        players.at(i)->accept->setEnabled(i == currentPlayer?true:false);
        players.at(i)->lSpinBox->setEnabled(i == currentPlayer?true:false);
    }
        numOfPlayers++;
    lay->update();
}

void LeaderBoardWidget::newPlayer()
{
    qDebug()<<"NewPlayer in LeaderBoardWidget";
    qDebug()<< "newPlayer!";
    //playerCreationWidget->addPlayer();
    playerCreationWidget->show();
    //addPlayer(newWidget);
    //Open Player Addition Widget
}

