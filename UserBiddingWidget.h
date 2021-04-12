#ifndef USERBIDDINGWIDGET_H
#define USERBIDDINGWIDGET_H
#define MAX_BID 99
#define MIN_BID 1
#define BID_BTN_TEXT "Accept Bid"

#include "user.h"
#include "LeaderBoardWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDate>
#include <QSpinBox>
#include <QColor>
#include <QUuid>
#include <QDateTime>

class UserBiddingWidget : public LeaderBoardWidget //public QWidget
{
    Q_OBJECT
public:
    explicit UserBiddingWidget(User* u/*, QWidget *parent = nullptr*/);
    QPushButton * accept = new QPushButton(this); //bidBtn in onlineWidget
    QSpinBox * lSpinBox = new QSpinBox(this); //biddingBox in onlineWidget
    QUuid getId();
private:
    QGridLayout * userLayout = new QGridLayout(this);
    QLabel * labelName = new QLabel(this); //name in onlineWidget
    QLabel * pointsName = new QLabel(this);
    User* user;
    void updateLayout();
public slots:
    void btnPressed();
    void resetBidding();
    void deactivateBtn();

signals:
    void biddingChanged(int userBidding, unsigned long timeStamp, QUuid id);
    void biddingReset(int playerBidding, QUuid id);
};

#endif // PLAYERBIDDINGWIDGET_H
