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
    u_int8_t numOfPlayers = 0;
    u_int8_t currentPlayer = 0;
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
