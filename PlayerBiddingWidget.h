#ifndef PLAYERBIDDINGWIDGET_H
#define PLAYERBIDDINGWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDate>
#include <QSpinBox>

class PlayerBiddingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerBiddingWidget(QWidget *parent = nullptr);
    QPushButton * accept = new QPushButton(this);
    QSpinBox  * lSpinBox = new QSpinBox(this);
private:
    QGridLayout * playerLayout = new QGridLayout(this);
    QLabel * labelName = new QLabel(this);
    QString playerName;
    unsigned int playerBidding;
    bool hasBid = 0;
public slots:
    void setBidding(QVariant v);
    void setName(QVariant v);
    void btnPressed();
    unsigned int getBidding();
    QString getName();

signals:
    void biddingChanged(unsigned int playerBidding);
    void nameChanged();
};

#endif // PLAYERBIDDINGWIDGET_H
