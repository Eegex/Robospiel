#ifndef PLAYERBIDDINGWIDGET_H
#define PLAYERBIDDINGWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDate>
#include <QDoubleSpinBox>
#include "LeaderBoardWidget.h"

class PlayerBiddingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerBiddingWidget(QWidget *parent = nullptr);

private:
    QGridLayout * playerLayout = new QGridLayout(this);
    QDoubleSpinBox  * lSpinBox = new QDoubleSpinBox(this);
    QString playerName;
    u_int16_t playerBidding;
public slots:
    virtual void setBidding(QVariant v);
    virtual void setName(QVariant v);
    virtual u_int getBidding();
    virtual QString getName();

signals:
    virtual void biddingChanged();
    virtual void nameChanged();
};

#endif // PLAYERBIDDINGWIDGET_H
