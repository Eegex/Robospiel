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
#include "user.h"
#define MAX_BID 99
#define BID_BTN_TEXT tr("Accept Bid")

class UserOnlineWidget : public QWidget
{
	Q_OBJECT
public:
	explicit UserOnlineWidget(QWidget *parent = nullptr);
    void setOnlineWidget();
	void addUserToList(User *u);
    void updateUserList();
    void setUserID(QUuid uid);
    void setTable();
private:
	QGridLayout * lay = new QGridLayout(this);
	QPushButton * bidBtn = new QPushButton(this); //accept in biddingWidget
	QLabel * name = new QLabel(this); //labelName in biddingWdiget
	QSpinBox * biddingBox = new QSpinBox(this); //lSpinBox in biddingWidget
	QTableWidget * listWidget = new QTableWidget(this);
    int userBidding = MAX_BID;
	QUuid userId;
    QString username = "Player";
	QColor usercolor;
	QVector<User*> users;
public slots:
	void btnPressed();
    void setBidding(QUuid id, int bidding);
    void updateName(QString newName); // TODO: same for color
signals:
	void userAdded(struct UserData * newUser);
	void biddingChangedOnline(int userBidding);
};

#endif // USERONLINEWIDGET_H