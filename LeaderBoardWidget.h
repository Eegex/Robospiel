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
    UserCreationWidget * userCreationWidget = new UserCreationWidget(nullptr);
    QVector<UserBiddingWidget*> players; //Several Players, Array of Widgets for individual players
    void addPlayer(User * newUser);

private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addBtn = new QPushButton(this);
    unsigned int numOfPlayers = 0;
    unsigned int currentPlayer = 0;

public slots:
    void sortByBidding();
    void newPlayer();

signals:

};

#endif // LEADERBOARDWIDGET_H
