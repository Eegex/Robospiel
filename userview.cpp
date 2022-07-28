#include "userview.h"

UserView* UserView::instance = nullptr;

/*!
 * \brief UserView::UserView This is the UI-Widget where the user can choose between a local game and being a client or server.
 * \param parent
 */
UserView::UserView(QPushButton *actionBtn, QPushButton *serverSwitchBtn, QWidget *parent) : QWidget(parent)
{
	network = new NetworkView();
	connect(network, &NetworkView::leaderboardOnline, this, [=]()
	{
		User* user = nullptr;
		if(leaderboard)
		{
			OnlineLeaderboardWidget* onlineLeaderboard = dynamic_cast<OnlineLeaderboardWidget*>(leaderboard);
			user = onlineLeaderboard->getLocalUser();
			onlineLeaderboard->deleteLater(); //TODO bleibt der User erhalten, auch wenn das Leaderboard glöscht wird?
		}
		leaderboard = new OnlineLeaderboardWidget();
		GameControll::getInstance().setLeaderboard(leaderboard);

		OnlineLeaderboardWidget* onlineLeaderboard = dynamic_cast<OnlineLeaderboardWidget*>(leaderboard);
		if(onlineLeaderboard)
		{
			onlineLeaderboard->setLocalUser(GameControll::getInstance().initializeUser(user));
			onlineLeaderboard->initialize();
		}

		network->hide();
		btnBack->show();
		layout->addWidget(leaderboard, 0, 0);
		actionBtn->setEnabled(true);
		serverSwitchBtn->setEnabled(true);
	});
	connect(network, &NetworkView::leaderboardOffline, this, [=]()
	{
		leaderboard = new OfflineLeaderBoardWidget();
		GameControll::getInstance().setLeaderboard(leaderboard);
		GameControll::addDefaultUsers();

		network->hide();
		layout->addWidget(leaderboard, 0, 0);
		btnBack->show();
	});
	connect(btnBack, &QPushButton::pressed, this, [=](){
		disconnectFromServer();
	});
	layout = new QGridLayout(this);
	layout->addWidget(network, 0, 0);
	layout->addWidget(btnBack, 1, 0);
	btnBack->hide();

	instance = this;
}

LeaderBoardWidget *UserView::getLeaderboard() const
{
	return leaderboard;
}

void UserView::disconnectFromServer()
{
	instance->layout->removeWidget(instance->leaderboard);
	if(instance->getLeaderboard())
	{
		instance->leaderboard->deleteLater();
		instance->leaderboard = nullptr;
	}
	GameControll::clearUsers();


	instance->network->toChoiceMenu();
	instance->network->show(); //TODO: ausreichend?
	instance->btnBack->hide();

	Client::getInstance().closeClient();

	//		GameControll::getInstance().setLeaderboard(leaderboard);
}


