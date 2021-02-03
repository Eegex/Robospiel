#include <QDebug>
#include "LeaderBoardWidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"
//contains the offline and online versions of the user-table, as well as the networkview to choose between them and establish network connections.
LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
	//userCreationWidget->hide();
	userOnlineWidget->hide();
	//lay->addWidget(userCreationWidget, numOfUsers, 0);
	connect(networkView, &NetworkView::leaderboardOnline, this, &LeaderBoardWidget::goOnline);
	connect(networkView, &NetworkView::leaderboardOffline, this, &LeaderBoardWidget::goOffline);
	if(isOnline == undecided)
	{
		lay->addWidget(networkView);
	}
	setLayout(lay);
}

void LeaderBoardWidget::goOnline()
{
	isOnline = online;
	networkView->hide();
	lay->addWidget(userOnlineWidget);
	userOnlineWidget->show();
	lay->update();
	emit onlineUserAdded(new User(username, usercolor, this));
}

void LeaderBoardWidget::goOffline()
{
	qDebug()<<"Called Function goOffline";
	isOnline = offline;
	networkView->hide();
	lay->addWidget(offlineLeaderBoardWidget);
	//userCreationWidget->show();
	// addBtn->setText("Add new User");
	// addBtn->show();
	lay->update();
}

void LeaderBoardWidget::goUndefined()
{
	if(isOnline == online)
	{
		lay->removeWidget(userOnlineWidget);
	}
	else if(isOnline == offline)
	{
		for(int i = 0; i < lay->count(); i++)
		{
			QWidget * tmp = lay->itemAt(i)->widget();
			if(tmp)
			{
				tmp->hide();
			}
		}
	}
	lay->addWidget(networkView);
	lay->update();
	isOnline = undecided;
}

void LeaderBoardWidget::setUsername(QString name)
{
	username = name;
	userOnlineWidget->updateName(username);
}

void LeaderBoardWidget::setUsercolor(QColor color)
{
	usercolor = color;
}

void LeaderBoardWidget::setOfflineLeaderBoardWidget(OfflineLeaderBoardWidget *value)
{
	offlineLeaderBoardWidget = value;
}

unsigned int LeaderBoardWidget::getNumOfUsers()
{
	return numOfUsers;
}

OfflineLeaderBoardWidget *LeaderBoardWidget::getOfflineLeaderBoardWidget()
{
	return offlineLeaderBoardWidget;
}

state LeaderBoardWidget::getOnlineState()
{
	return isOnline;
}

QVector<UserBiddingWidget*>* LeaderBoardWidget::getUsers()
{
	return &users;
}

UserCreationWidget *LeaderBoardWidget::getUserCreationWidget()
{
	return userCreationWidget;
}

UserOnlineWidget *LeaderBoardWidget::getUserOnlineWidget()
{
	return userOnlineWidget;
}

NetworkView *LeaderBoardWidget::getNetworkView()
{
	return networkView;
}

QString LeaderBoardWidget::getUsername()
{
	return username;
}

QColor LeaderBoardWidget::getUsercolor()
{
	return usercolor;
}
