#ifndef PLAYERBIDDINGWIDGET_H
#define PLAYERBIDDINGWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDate>
#include <QDoubleSpinBox>

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
    void setBidding(QVariant v);
    void setName(QVariant v);
    u_int getBidding();
    QString getName();

signals:
    void biddingChanged();
    void nameChanged();
};

#endif // PLAYERBIDDINGWIDGET_H
