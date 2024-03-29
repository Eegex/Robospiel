#ifndef ONLINELEADERBOARDWIDGET_H
#define ONLINELEADERBOARDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QUuid>
#include <QTableView>
#include "powerbuttondelegate.h"
#include "spinbox.h"
#include "Direction.h"
#include "LeaderBoardWidget.h"
#include "user.h"
#include "gamecontroll.h"
#include "tablemodel.h"
#include "votekickdelegate.h"

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
	void handleActionButtonRelease();
	void updateActionBtnText();
	void enableActionBtn(bool boolean);
private:
	QGridLayout * lay = new QGridLayout(this);
	QPushButton * bidBtn = new QPushButton(this); //accept in biddingWidget
	QLabel * lname = new QLabel(this); //labelName in biddingWdiget
	QLabel * lplayerInPower = new QLabel(this);
	QLabel * lserver = new QLabel(this);
	SpinBox * biddingBox = new SpinBox(this); // lSpinBox in biddingWidget
	QTableView * tableView = new QTableView(this);
	int userBidding = User::maxBid;
	User * localUser = nullptr;
	TableModel * model = new TableModel(this);
	PowerButtonDelegate * powerButtonDelegate = nullptr;
	VotekickDelegate* votekickDelegate = nullptr;

	QPushButton * actionBtn = new QPushButton(this); //aka. Vote-Btn
	QMap<GameControll::Phase, QString> actionBtnTexts;
	QLabel * dlSteps = new QLabel(this);

	bool tooltipDir = false;

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
	void votekickClicked(const QModelIndex& index);
};

#endif // ONLINELEADERBOARDWIDGET_H
