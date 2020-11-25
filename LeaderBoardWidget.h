#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include "PlayerBiddingWidget.h"

class LeaderBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeaderBoardWidget(QWidget *parent = nullptr);

private:
    QGridLayout * lay = new QGridLayout(this);
    QLabel * lDisplay = new QLabel("LeaderBoardWidget", this);
    u_int8_t numOfPlayers = 5;
    PlayerBiddingWidget * dummyWidget[]; //Several Players, Array of pointers to Widgets for individual players

public slots:
    virtual void sortByBidding();

signals:

};

#endif // LEADERBOARDWIDGET_H
