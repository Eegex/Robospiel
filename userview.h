#ifndef USERVIEW_H
#define USERVIEW_H

#include "LeaderBoardWidget.h"
#include "offlineleaderboardwidget.h"
#include "onlineleaderboardwidget.h"
#include <QWidget>
#include "networkview.h"

class UserView : public QWidget
{
    Q_OBJECT
public:
    explicit UserView(QWidget *parent = nullptr);
    LeaderBoardWidget *getLeaderboard() const;

private:
    LeaderBoardWidget* leaderboard;
    NetworkView* network;
    QGridLayout* layout;
    QPushButton* btnBack = new QPushButton(tr("Back"));

signals:

};

#endif // USERVIEW_H
