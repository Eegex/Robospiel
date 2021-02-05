#ifndef OFFLINELEADERBOARDWIDGET_H
#define OFFLINELEADERBOARDWIDGET_H
#define MAX_USERS 10

#include <QObject>
#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QVector>
#include <QSpinBox>
#include <QDateTime>
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"



class OfflineLeaderBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OfflineLeaderBoardWidget(QWidget *parent = nullptr);
    void addUser(User * newUser);
    QVector<UserBiddingWidget*> * getUsers();
    UserCreationWidget *getUserCreationWidget();
    unsigned int getBiddingWidgetIndexByID(QUuid id);
    unsigned int getNumOfUsers();
    //void setUsername(QString name);
    //void setUsercolor(QColor color);
    QString getUsername();
    QColor getUsercolor();
private:
    QGridLayout * lay = new QGridLayout(this);
    unsigned int numOfUsers = 0;
    unsigned int currentUser = 0;
    QVector<UserBiddingWidget*> users; //Several Users, Array of Widgets for individual users
    UserCreationWidget * userCreationWidget = new UserCreationWidget(nullptr);
public slots:
    void updateLayout();
    //void newUser();

signals:
    void userAdded(struct Userdata * newUser);

};

#endif // OFFLINELEADERBOARDWIDGET_H
