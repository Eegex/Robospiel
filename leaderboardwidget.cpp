#include <QDebug>
#include "LeaderBoardWidget.h"
#include "PlayerBiddingWidget.h"
#include "PlayerCreationWidget.h"
#include "user.h"

LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
    playerCreationWidget->hide();
    addBtn->setText("Add new Player");
    lay->addWidget(addBtn, numOfPlayers, 0);
    setLayout(lay);
    connect(addBtn,&QPushButton::clicked,this,&LeaderBoardWidget::newPlayer);
    connect(playerCreationWidget, &PlayerCreationWidget::playerAdded, this, &LeaderBoardWidget::addPlayer);
}

void LeaderBoardWidget::sortByBidding()
{
    //TODO: Will be done in back end
}

void LeaderBoardWidget::addPlayer(struct user * newUser)
{
    qDebug()<<"LeaderBoardWidget: AddPlayer: Add player with name: "<<newUser->name<<"and colour "<<(newUser->colour.isValid()?newUser->colour.name():"0x000000");
    PlayerBiddingWidget * newWidget = new PlayerBiddingWidget(this); //Create new BiddingWidget to display
    newWidget->setName(newUser->name);
    newWidget->setColor(newUser->colour.name());
    players.append(newWidget); //Append widget to list of players
    lay->addWidget(players.at(numOfPlayers));
    numOfPlayers++; //Increment number of players, important for correct placement of button
    for(unsigned int i = 0; i<numOfPlayers; i++){lay->addWidget(players.at(i), i, 0);}
    lay->addWidget(addBtn, numOfPlayers, 0); //New Player Button under all Players
    lay->update();
}

void LeaderBoardWidget::newPlayer()
{
    qDebug()<<"NewPlayer in LeaderBoardWidget";
    playerCreationWidget->show(); //Show creation widget
    playerCreationWidget->setFocus(); //Set focus to specific window
}

