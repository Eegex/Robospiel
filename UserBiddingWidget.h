#ifndef USERBIDDINGWIDGET_H
#define USERBIDDINGWIDGET_H

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
    QPushButton * accept = new QPushButton(this);
    QSpinBox * lSpinBox = new QSpinBox(this);
private:
    QGridLayout * userLayout = new QGridLayout(this);
    QLabel * labelName = new QLabel(this);
    QUuid userId;
    QString userName;
    QColor userColor;
    int userBidding = 999;
public slots:
    QUuid getId();
    QString getName();
    int getBidding();
    void setId(QUuid id);
    void setBidding(int bidding);
    void setName(QString v);
    void setColor(QColor colour);
    void btnPressed();

signals:
    void biddingChanged(int userBidding, QUuid id);
    void nameChanged();
    void colourChanged();
};

#endif // PLAYERBIDDINGWIDGET_H
