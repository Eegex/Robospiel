#include "onlineleaderboardwidget.h"
#include <QDebug>
#include <QBrush>
#include <QHeaderView>

OnlineLeaderboardWidget::OnlineLeaderboardWidget()
{

}

void OnlineLeaderboardWidget::initialize()
{
	bidBtn->hide();
	lname->hide();
	lplayerInPower->hide();
	lserver->hide();
	biddingBox->hide();
	tableView->hide();
	setOnlineWidget();
	setLayout(lay);
	biddingBox->setSelectAllOnFocus(true);
	tableView->setModel(model);
	tableView->resizeColumnsToContents();
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	powerButtonDelegate = new PowerButtonDelegate({tableView->horizontalHeader()->sectionSize(0),tableView->verticalHeader()->sectionSize(0)},this);
	connect(tableView->horizontalHeader(),&QHeaderView::sectionResized,powerButtonDelegate,&PowerButtonDelegate::updateSizeHint);
	tableView->setItemDelegateForColumn(0, powerButtonDelegate);
	connect(tableView, &QTableView::entered,powerButtonDelegate,&ButtonDelegate::handleHover);
	connect(tableView, &QTableView::clicked,powerButtonDelegate,&ButtonDelegate::handleClick);
	connect(powerButtonDelegate, &ButtonDelegate::buttonClicked,this,&OnlineLeaderboardWidget::userClicked);

	int lastIndex = tableView->horizontalHeader()->count()-1;
	votekickDelegate = new VotekickDelegate({tableView->horizontalHeader()->sectionSize(lastIndex),tableView->verticalHeader()->sectionSize(0)},this);
	connect(tableView->horizontalHeader(),&QHeaderView::sectionResized,votekickDelegate,&VotekickDelegate::updateSizeHint);
	tableView->setItemDelegateForColumn(lastIndex, votekickDelegate);
	connect(tableView, &QTableView::entered,votekickDelegate,&ButtonDelegate::handleHover);
	connect(tableView, &QTableView::clicked,votekickDelegate,&ButtonDelegate::handleClick);
	connect(votekickDelegate, &ButtonDelegate::buttonClicked,this,&OnlineLeaderboardWidget::votekickClicked);

	biddingBox->setSpecialValueText(tr("No Bid"));
	connect(bidBtn,&QPushButton::clicked, this, &OnlineLeaderboardWidget::btnPressed);
	connect(biddingBox,&SpinBox::returnPressed, this,  &OnlineLeaderboardWidget::btnPressed);
	//connect(this, &OnlineLeaderboardWidget::updateLayout, tableView, &QTableWidget::clearContents);
}

void OnlineLeaderboardWidget::userClicked(const QModelIndex & index)
{
	QTimer::singleShot(300,this,[=]()
	{
		powerButtonDelegate->resetState();
		emit model->dataChanged(model->index(0,0),model->index(GameControll::getUsers()->size(),0));
	});
	emit userWasClicked(GameControll::getUsers()->at(index.row())->getId());
}

void OnlineLeaderboardWidget::votekickClicked(const QModelIndex& index)
{
	QTimer::singleShot(300,this,[=]()
	{
		votekickDelegate->resetState();
		emit model->dataChanged(index, index);
	});
	QJsonObject data;
	data.insert("fromUser", getLocalUser()->getId().toString());
	data.insert("toUser", GameControll::getUsers()->at(index.row())->getId().toString());
	GameControll::triggerActionWithData(PlayerAction::votekick, data);
}

User * OnlineLeaderboardWidget::getLocalUser() const
{
	return localUser;
}

void OnlineLeaderboardWidget::setOnlineWidget()
{
	biddingBox->setMinimum(1);
	biddingBox->setSingleStep(1);
	bidBtn->setText(BID_BTN_TEXT);
	lay->addWidget(lplayerInPower);
	lay->addWidget(lserver);
	lay->addWidget(lname);
	lay->addWidget(biddingBox);
	lay->addWidget(bidBtn);
	lay->addWidget(tableView);
	lname->setText(localUser->getName());
	lplayerInPower->setText("");
	lserver->setText("Server: You");
	lname->show();
	lplayerInPower->show();
	lserver->show();
	tableView->show();
	biddingBox->show();
	bidBtn->show();
	lay->update();
	emit leaderBoardSet(true);
}

void OnlineLeaderboardWidget::setLocalUser(User * u)
{
	if(localUser)
	{
		delete localUser;
	}
	localUser = u;
}

void OnlineLeaderboardWidget::btnPressed()
{
	if(biddingBox->value() > 1)
	{
		biddingBox->setMaximum(userBidding = biddingBox->value());
		bidBtn->setText(tr("Bid") + ":" + QString::number(userBidding));
		emit biddingAccepted(localUser->getId(), userBidding);
		setBiddingFocus();
	}
}

void OnlineLeaderboardWidget::newRound(){
	//TODO
}

/*!
 * \brief OnlineLeaderboardWidget::updateBidding ist super
 * \param id
 * \param bidding
 */
void OnlineLeaderboardWidget::updateBidding(QUuid id, int bidding)
{
	userBidding = bidding; // notwendig?
	emit biddingChangedOnline(userBidding);
	model->update();
	setBiddingFocus();
}

void OnlineLeaderboardWidget::updateName(QUuid id, QString newName)
{
	model->update();
	if(localUser->getId() == id)
	{
		//localUser->setName(newName);
		lname->setText(newName);
	}
	//lname->setText(newName);
}

void OnlineLeaderboardWidget::updatePlayerInPower(QUuid id, QString newName)
{
	model->update();
	if(id == localUser->getId())
	{
		newName = "YOU!!!";
	}
	lplayerInPower->setText(tr("playing: ") + newName);
}

void OnlineLeaderboardWidget::noPlayerInPower()
{
	lplayerInPower->setText(" - ");
}

void OnlineLeaderboardWidget::updateServerName(QUuid id, QString newName)
{
	model->update();
	if(id == localUser->getId())
	{
		newName = "YOU!!!";
	}
	lserver->setText(tr("Server: ") + newName);
}

void OnlineLeaderboardWidget::updateColour(QUuid id, QColor color)
{
	model->update();
}

void OnlineLeaderboardWidget::addUser(User *u)
{
	connect(u,&User::biddingChanged,this,[&](QUuid id, int bid)
	{
		model->update();
		if(id == localUser->getId())
		{
			biddingBox->setMaximum(std::min(bid,User::maxBid));
			if(!localUser->getHasBid())
			{
				bidBtn->setText(tr("Accept Bid"));
				biddingBox->setValue(1);
			}
			else
			{
				setBiddingFocus();
			}
		}
	});
	connect(u, &User::pointsChanged, this, [=]()->void
	{
		model->update();
	});
	model->updateUsers();
}

void OnlineLeaderboardWidget::deactivateInput()
{
	/*if(localUser->hasBid)
	{
		localUser->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
	} */
	bidBtn->setEnabled(false);
	biddingBox->setEnabled(false);
}

void OnlineLeaderboardWidget::activateInput()
{
	bidBtn->setEnabled(true);
	biddingBox->setEnabled(true);
	setBiddingFocus();
}

void OnlineLeaderboardWidget::setBiddingFocus()
{
	biddingBox->setFocus();
	if(biddingBox->getSelectAllOnFocus())
	{
		biddingBox->selectAll(); //Wird benötigt, weil das implementierte SpinBox::focusInEvent nicht aufgerufen wird, wenn der Fokus schon auf der Spinbox ist.
	}
}

void OnlineLeaderboardWidget::updateAllUsers()
{
	model->updateUsers();
}

void OnlineLeaderboardWidget::setFreeplayButtonsVisible(bool visible)
{
	if(powerButtonDelegate)
	{
		powerButtonDelegate->setButtonVisible(visible);
	}
	tableView->update();
}
