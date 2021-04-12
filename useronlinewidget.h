#ifndef USERONLINEWIDGET_H
#define USERONLINEWIDGET_H

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
#define MAX_BID 99

class UserOnlineWidget : public LeaderBoardWidget
{
	Q_OBJECT
public:
    explicit UserOnlineWidget();
    void setOnlineWidget();
	void addUserToList(User *u);
    void updateUserList();
    void setUserID(QUuid uid);
    void setTable();
private:
    QGridLayout * lay = new QGridLayout(this);
	QPushButton * bidBtn = new QPushButton(this); //accept in biddingWidget
    QLabel * lname = new QLabel(this); //labelName in biddingWdiget
	QSpinBox * biddingBox = new QSpinBox(this); //lSpinBox in biddingWidget
	QTableWidget * listWidget = new QTableWidget(this);
    int userBidding = MAX_BID;
	QUuid userId;
    QString username = "Player"; //for testing
    QColor usercolor = QColor(Qt::green);// green for testing
	QVector<User*> users;

    User * findUser(QUuid id);
public slots:
	void btnPressed();
    // void setBidding(QUuid id, int bidding); before
    void updateBidding(QUuid id, int bidding);
    // void updateName(QString newName); before
    void updateName(QUuid id, QString name);
    void updateColor(QUuid id, QColor color);
    void deactivateInput(); //TODO
    void updateAllUsers(); //TODO
signals:
    // void biddingChangedOnline(int userBidding); before
    void userAdded(User* u); //TODO
    void biddingAccepted(QUuid userId, int bidding); // before: biddingChangedOnline
};

#endif // USERONLINEWIDGET_H
