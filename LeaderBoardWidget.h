#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H
#define MAX_USERS 10
#define MIN_BID 1
#define BID_BTN_TEXT tr("Accept Bid")

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QVector>
#include <QSpinBox>
#include <QDateTime>
#include <QString>
#include "user.h"
#include "networkview.h"

class LeaderBoardWidget : public QWidget
{
	Q_OBJECT
public:
	virtual void addUser(User* user) = 0;

public slots:
	virtual void updateBidding(QUuid id, int bidding) = 0;
	virtual void updateName(QUuid id, QString name) = 0;
	virtual void updateColour(QUuid id, QColor color) = 0;
	virtual void deactivateInput() = 0;
	virtual void activateInput() = 0;
	virtual void updateAllUsers() = 0; //Change List to reflect current game status
	virtual void setBiddingFocus() = 0;

signals:
	void biddingAccepted(QUuid userId, int bidding);
    void userWasClicked(QUuid userId);
};

#endif // LEADERBOARDWIDGET_H




