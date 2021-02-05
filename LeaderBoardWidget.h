#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H
#define MAX_USERS 10

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QVector>
#include <QSpinBox>
#include <QDateTime>
#include <QString>
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "useronlinewidget.h"
#include "user.h"
#include "networkview.h"
#include "offlineleaderboardwidget.h"

class LeaderBoardWidget : public QWidget
{
	Q_OBJECT
public:
    virtual void addUser(User* user) = 0;

public slots:
    virtual void updateBidding(QUuid id, int bidding) = 0;
    virtual void updateName(QUuid id, QString name) = 0;
    virtual void updateColor(QUuid id, QColor color) = 0;
    virtual void deactivateInput() = 0;
    virtual void updateAllUsers() = 0;

signals:
    virtual void userAdded(User* u) = 0;
    virtual void biddingAccepted(QUuid userId, int bidding) = 0;
};

#endif // LEADERBOARDWIDGET_H




