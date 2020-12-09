#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QVector>
#include "PlayerBiddingWidget.h"
#include "PlayerCreationWidget.h"
#include "user.h"

class LeaderBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeaderBoardWidget(QWidget *parent = nullptr);

private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addBtn = new QPushButton(this);
    unsigned int numOfPlayers = 0;
    unsigned int currentPlayer = 0;
    //PlayerBiddingWidget* players;
    QVector<PlayerBiddingWidget*> players; //Several Players, Array of Widgets for individual players
    PlayerCreationWidget * playerCreationWidget = new PlayerCreationWidget(nullptr);

public slots:
    void sortByBidding();
    void addPlayer(User * newUser);
    void newPlayer();

signals:

};

#endif // LEADERBOARDWIDGET_H
