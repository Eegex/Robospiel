#ifndef ONLINELEADERBOARDWIDGET_H
#define ONLINELEADERBOARDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QUuid>
#include <QSpinBox>
#include <QTableWidget>
#include "Direction.h"
#include "LeaderBoardWidget.h"
#include "user.h"
#include "gamecontroll.h"

class OnlineLeaderboardWidget : public LeaderBoardWidget
{
	Q_OBJECT
public:
    explicit OnlineLeaderboardWidget();
    void setOnlineWidget();
    void addUser(User *u);
    void updateUserList();
    void setTable();
    void setLocalUser(User *u);
    void initialize();
private:
    QGridLayout * lay = new QGridLayout(this);
	QPushButton * bidBtn = new QPushButton(this); //accept in biddingWidget
    QLabel * lname = new QLabel(this); //labelName in biddingWdiget
	QSpinBox * biddingBox = new QSpinBox(this); //lSpinBox in biddingWidget
	QTableWidget * listWidget = new QTableWidget(this);
    int userBidding = MAX_BID;
    User * localUser;
	QVector<User*> users;

    User * findUser(QUuid id);
public slots:
	void btnPressed();
    // void setBidding(QUuid id, int bidding); before
    void updateBidding(QUuid id, int bidding);
    // void updateName(QString newName); before
    void updateName(QUuid id, QString name);
	void updateColour(QUuid id, QColor color);
    void deactivateInput(); //TODO
    void updateAllUsers(); //TODO
	void activateInput(); //TODO
signals:
    // void biddingChangedOnline(int userBidding); before
    void userAdded(User* u); //TODO
    //void biddingAccepted(QUuid userId, int bidding); // before: biddingChangedOnline
};

#endif // ONLINELEADERBOARDWIDGET_H
