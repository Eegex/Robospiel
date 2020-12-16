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
    QSpinBox * lSpinBox = new QSpinBox(this);
private:
    QGridLayout * playerLayout = new QGridLayout(this);
    QLabel * labelName = new QLabel(this);
    QString playerName;
    QColor playerColor;
    int playerBidding = 999;
public slots:
    QString getName();
    int getBidding();
    void setBidding(int bidding);
    void setName(QString v);
    void setColor(QColor colour);
    void btnPressed();

signals:
    void biddingChanged(int playerBidding);
    void nameChanged();
    void colourChanged();
};

#endif // PLAYERBIDDINGWIDGET_H
