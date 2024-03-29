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
#include "LeaderBoardWidget.h"
#include "UserCreationWidget.h"
#include "user.h"
#include "gamecontroll.h"



class OfflineLeaderBoardWidget : public LeaderBoardWidget
{
	Q_OBJECT
public:
	explicit OfflineLeaderBoardWidget();
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
	void updateBidding(QUuid id, int bidding);
	void updateName(QUuid id, QString name);
    void updatePlayerInPower(QUuid id, QString name);
    void updateServerName(QUuid id, QString name);
    void noPlayerInPower();
	void updateColour(QUuid id, QColor colour);
	void updateAllUsers();
	void newRound();
	void deactivateInput();
	void activateInput();
	void updateLayout();
	void setBiddingFocus();
	//void newUser();
	void setFreeplayButtonsVisible(bool visible);
};

#endif // OFFLINELEADERBOARDWIDGET_H
