#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QVector>
#include <QSpinBox>
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "useronlinewidget.h"
#include "user.h"
#include "networkview.h"

class LeaderBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeaderBoardWidget(QWidget *parent = nullptr);
    void addUser(User * newUser);
    QVector<UserBiddingWidget*> * getUsers();
    UserCreationWidget *getUserCreationWidget();
    UserOnlineWidget *getUserOnlineWidget();
    unsigned short getIsOnline();
    int userBidding = 999;
    void goOnline();
    void goOffline();
    void goUndefined();

private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addBtn = new QPushButton(this);
    unsigned int numOfUsers = 0;
    unsigned int currentUser = 0;
    unsigned int isOnline = 0; // 0 = not decided, 1 = offline, 2 = online
    QVector<UserBiddingWidget*> users; //Several Users, Array of Widgets for individual users
    UserCreationWidget * userCreationWidget = new UserCreationWidget(nullptr);
    UserOnlineWidget * userOnlineWidget = new UserOnlineWidget(nullptr);
    NetworkView * networkView = new NetworkView(nullptr);


public slots:
    void sortByBidding();
    void newUser();
signals:
    void userAdded(struct Userdata * newUser);

};

#endif // LEADERBOARDWIDGET_H


