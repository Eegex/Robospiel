#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H
#define MAX_USERS 10

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

enum state {undecided, offline, online};

class LeaderBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeaderBoardWidget(QWidget *parent = nullptr);
    void addUser(User * newUser);
    QVector<UserBiddingWidget*> * getUsers();
    UserCreationWidget *getUserCreationWidget();
    unsigned int getBiddingWidgetIndexByID(QUuid id);
    UserOnlineWidget *getUserOnlineWidget();
    unsigned short getIsOnline();
    unsigned int getNumOfUsers();
    void goOnline();
    void goOffline();
    void goUndefined();
    void setUsername(QString name);
    void setUsercolor(QColor color);
    QString getUsername();
    QColor getUsercolor();
    NetworkView *getNetworkView();
private:
    QGridLayout * lay = new QGridLayout(this);
    //QPushButton * addBtn = new QPushButton(this);
    QString username = "Hans"; // for testing in MacOs
    QColor usercolor = QColor(168, 218, 173); // for testing in MacOs
    unsigned int numOfUsers = 0;
    unsigned int currentUser = 0;
    unsigned int isOnline = undecided; // 0 = not decided, 1 = offline, 2 = online
    QVector<UserBiddingWidget*> users; //Several Users, Array of Widgets for individual users
    UserCreationWidget * userCreationWidget = new UserCreationWidget(nullptr);
    UserOnlineWidget * userOnlineWidget = new UserOnlineWidget(nullptr);
    NetworkView * networkView = new NetworkView(nullptr);


public slots:
    void sortByBidding();
    void updateLayout();
    void newUser();
signals:
    void userAdded(struct Userdata * newUser);

};

#endif // LEADERBOARDWIDGET_H


