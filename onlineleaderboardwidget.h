#ifndef ONLINELEADERBOARDWIDGET_H
#define ONLINELEADERBOARDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QUuid>
#include <QTableView>
#include "spinbox.h"
#include "Direction.h"
#include "LeaderBoardWidget.h"
#include "user.h"
#include "gamecontroll.h"
#include "tablemodel.h"
#include "leaderboarddelegate.h"

/*!
 * \brief The OnlineLeaderboardWidget class ist auch super
 */
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
	void updateServerName(QUuid id, QString newName);
private:
	QGridLayout * lay = new QGridLayout(this);
	QPushButton * bidBtn = new QPushButton(this); //accept in biddingWidget
	QLabel * lname = new QLabel(this); //labelName in biddingWdiget
    QLabel * lplayerInPower = new QLabel(this);
    QLabel * lserver = new QLabel(this);
	SpinBox * biddingBox = new SpinBox(this); //lSpinBox in biddingWidget
	QTableView * tableView = new QTableView(this);
	int userBidding = User::maxBid;
	User * localUser = nullptr;
	TableModel * model = new TableModel(this);
	LeaderboardDelegate * delegate = nullptr;

public slots:
	void btnPressed();
	// void setBidding(QUuid id, int bidding); // before
	void updateBidding(QUuid id, int bidding);
	// void updateName(QString newName); // before
	void newRound(); //TODO
	void updateName(QUuid id, QString name);
    void updatePlayerInPower(QUuid id, QString newName);
    void noPlayerInPower();
	void updateColour(QUuid id, QColor color);
	void deactivateInput();
	void updateAllUsers(); //TODO
	void activateInput();
	void setBiddingFocus();
	void setFreeplayButtonsVisible(bool visible);
signals:
	void biddingChangedOnline(int userBidding); // before
//	void biddingAccepted(QUuid userId, int bidding); // before: biddingChangedOnline
	void leaderBoardSet(bool t);
private slots:
	void userClicked(const QModelIndex & index);
};

#endif // ONLINELEADERBOARDWIDGET_H
