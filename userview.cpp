#include "userview.h"

//Container for NetworkView and online/offline-LeaderboardWidget
UserView::UserView(QWidget *parent) : QWidget(parent)
{
    network = new NetworkView();
    connect(network, &NetworkView::leaderboardOnline, this, [=](){
        leaderboard = new OnlineLeaderboardWidget();
        dynamic_cast<OnlineLeaderboardWidget*>(leaderboard)->setLocalUser(GameControll::getInstance().initializeUser());
        dynamic_cast<OnlineLeaderboardWidget*>(leaderboard)->initialize();
        network->hide();
        layout->addWidget(leaderboard, 0, 0);
        layout->addWidget(btnBack, 1, 0);
		GameControll::getInstance().setLeaderboard(leaderboard);
    });
	connect(network, &NetworkView::leaderboardOffline, this, [=](){
		leaderboard = new OfflineLeaderBoardWidget();
        //layout->removeWidget(network);
        network->hide();
		layout->addWidget(leaderboard, 0, 0);
		layout->addWidget(btnBack,1,0);
		GameControll::getInstance().setLeaderboard(leaderboard);
	});
    connect(btnBack, &QPushButton::pressed, this, [=](){
        // layout->removeWidget(leaderboard);
        // layout->removeWidget(btnBack);
        leaderboard->hide();
        btnBack->hide();
        layout->addWidget(network, 0, 0);
		GameControll::getInstance().setLeaderboard(leaderboard);
    });
    layout = new QGridLayout(this);
    layout->addWidget(network, 0, 0);
}

LeaderBoardWidget *UserView::getLeaderboard() const
{
    return leaderboard;
}
