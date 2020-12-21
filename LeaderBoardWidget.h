#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QVector>
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
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
    QVector<UserBiddingWidget*> players; //Several Players, Array of Widgets for individual players
    UserCreationWidget * playerCreationWidget = new UserCreationWidget(nullptr);

public slots:
    void sortByBidding();
    void addPlayer(struct UserData * newUser);
    void newPlayer();

signals:

};

#endif // LEADERBOARDWIDGET_H
