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
    void addPlayer(User * newUser);
    QVector<UserBiddingWidget*> * getUsers();
    UserCreationWidget *getUserCreationWidget();

private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addBtn = new QPushButton(this);
    unsigned int numOfPlayers = 0;
    unsigned int currentPlayer = 0;
    unsigned short isOnline = 0;
    QVector<UserBiddingWidget*> users; //Several Players, Array of Widgets for individual players
    UserCreationWidget * userCreationWidget = new UserCreationWidget(nullptr);

public slots:
    void sortByBidding();
    void newPlayer();
    void updateLayout();

signals:

};

#endif // LEADERBOARDWIDGET_H


