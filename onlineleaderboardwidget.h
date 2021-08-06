#ifndef ONLINELEADERBOARDWIDGET_H
#define ONLINELEADERBOARDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QUuid>
#include "spinbox.h"
#include <QTableView>
#include "Direction.h"
#include "LeaderBoardWidget.h"
#include "user.h"
#include "gamecontroll.h"
#include "tablemodel.h"

class OnlineLeaderboardWidget : public LeaderBoardWidget
{
	Q_OBJECT
public:
	explicit OnlineLeaderboardWidget();
	void setOnlineWidget();
	void addUser(User *u);
	// void updateUserList(); -> warum?
	void setLocalUser(User *u);
	void initialize();
	User * getLocalUser() const;

private:
	QGridLayout * lay = new QGridLayout(this);
	QPushButton * bidBtn = new QPushButton(this); //accept in biddingWidget
	QLabel * lname = new QLabel(this); //labelName in biddingWdiget
	SpinBox * biddingBox = new SpinBox(this); //lSpinBox in biddingWidget
	QTableView * tableView = new QTableView(this);
	int userBidding = User::maxBid;
	User * localUser = nullptr;
	TableModel * model = new TableModel(this);
public slots:
	void btnPressed();
	// void setBidding(QUuid id, int bidding); // before
	void updateBidding(QUuid id, int bidding);
	// void updateName(QString newName); // before
	void updateName(QUuid id, QString name);
	void updateColour(QUuid id, QColor color);
	void deactivateInput();
	void updateAllUsers(); //TODO
	void activateInput();
	void setBiddingFocus();
signals:
	void biddingChangedOnline(int userBidding); // before
	void userAdded(User* u); //TODO
//	void biddingAccepted(QUuid userId, int bidding); // before: biddingChangedOnline
    void leaderBoardSet(bool t);
};

#endif // ONLINELEADERBOARDWIDGET_H
