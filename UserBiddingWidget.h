#ifndef USERBIDDINGWIDGET_H
#define USERBIDDINGWIDGET_H
#define MAX_BID 99
#define BID_BTN_TEXT "Accept Bid"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDate>
#include <QSpinBox>
#include <QColor>
#include <QUuid>

class UserBiddingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserBiddingWidget(QWidget *parent = nullptr);
    QPushButton * accept = new QPushButton(this); //bidBtn in onlineWidget
    QSpinBox * lSpinBox = new QSpinBox(this); //biddingBox in onlineWidget
    bool active = true; //Needed for sorting
private:
    QGridLayout * userLayout = new QGridLayout(this);
    QLabel * labelName = new QLabel(this); //name in onlineWidget
    QLabel * pointsName = new QLabel(this);
    int userBidding = MAX_BID;
    int userPoints = 0;
    QUuid userId;
    QString userName;
    QColor userColor;
public slots:
    QUuid getId();
    QString getName();
    int getBidding();
    int getPoints();
    void setId(QUuid id);
    void setBidding(int bidding);
    void setName(QString v);
    void setColor(QColor colour);
    void incrementPoints();
    void btnPressed();
    void resetBidding();
    void deactivateBtn();

signals:
    void biddingChanged(int userBidding, QUuid id);
    void nameChanged();
    void colourChanged();
    void biddingReset(int playerBidding, QUuid id);
};

#endif // PLAYERBIDDINGWIDGET_H
