#include "userview.h"

//Container for NetworkView and online/offline-LeaderbordWidget
UserView::UserView(QWidget *parent) : QWidget(parent)
{
    network = new NetworkView();
    connect(network, &NetworkView::leaderboardOnline, this, [=](){
        leaderboard = new OnlineLeaderboardWidget();
        layout->removeWidget(network);
        layout->addWidget(leaderboard, 0, 0);
        layout->addWidget(btnBack, 1, 0);
    });
    connect(network, &NetworkView::leaderboardOffline, this, [=](){
        leaderboard = new OfflineLeaderboardWidget();
        layout->removeWidget(network);
        layout->addWidget(leaderboard, 0, 0);
        layout->addWidget(btnBack,1,0);
    });
    connect(btnBack, &QPushButton::pressed, this, [=](){
        layout->removeWidget(leaderboard);
        layout->removeWidget(btnBack);
        layout->addWidget(network, 0, 0);
    });
    layout = new QGridLayout(this);
    layout->addWidget(network, 0, 0);
}

LeaderBoardWidget *UserView::getLeaderboard() const
{
    return leaderboard;
}
