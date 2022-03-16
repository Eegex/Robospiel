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
	biddingBox->hide();
	tableView->hide();
	setOnlineWidget();
	setLayout(lay);
	biddingBox->setSelectAllOnFocus(true);
	tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	delegate = new LeaderboardDelegate({tableView->horizontalHeader()->sectionSize(0),tableView->verticalHeader()->sectionSize(0)},this);
	connect(tableView->horizontalHeader(),&QHeaderView::sectionResized,delegate,&LeaderboardDelegate::updateSizeHint);
    tableView->setItemDelegate(delegate);
	connect(tableView, &QTableView::entered,delegate,&LeaderboardDelegate::userHovered);
	connect(tableView, &QTableView::clicked,delegate,&LeaderboardDelegate::userClicked);
	connect(delegate, &LeaderboardDelegate::userBtnClicked,this,&OnlineLeaderboardWidget::userClicked);
	biddingBox->setSpecialValueText(tr("No Bid"));
	connect(bidBtn,&QPushButton::clicked, this, &OnlineLeaderboardWidget::btnPressed);
	connect(biddingBox,&SpinBox::returnPressed, this,  &OnlineLeaderboardWidget::btnPressed);
	//connect(this, &OnlineLeaderboardWidget::updateLayout, tableView, &QTableWidget::clearContents);
}

void OnlineLeaderboardWidget::userClicked(const QModelIndex & index)
{
	if(!index.column())
	{
		qDebug() << "Name clicked" << index;
		emit userWasClicked(GameControll::getUsers()->at(index.row())->getId());
	}
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
	lay->addWidget(lname);
	lay->addWidget(biddingBox);
	lay->addWidget(bidBtn);
	lay->addWidget(tableView);
	lname->setText(localUser->getName());
	lplayerInPower->setText("");
	lname->show();
	lplayerInPower->show();
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
		qDebug() << "Player changed their bidding to: " << userBidding;
	}
	else
	{
		qDebug() << "Someone has tried to bid No Bid";
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
	//qDebug() << "OnlineLeaderboardWidget::updateBidding(QUuid " << id << ", int " << bidding << ")";
	/*User * tmpUser = model->findUser(id);
	QString name = "";
	if (tmpUser != nullptr)
	{
		name = tmpUser->getName();
		for(QTableWidgetItem *item : tableView->findItems(name,0))
		{
			if(item->data(1) == id && item->text() == name)
			{
				tableView->item(item->row(),item->column()+1)->setText(QString::number(bidding));
				return;
			}
		}
	}*/
	userBidding = bidding; // notwendig?
	emit biddingChangedOnline(userBidding);
	model->update();
	setBiddingFocus();
}

void OnlineLeaderboardWidget::updateName(QUuid id, QString newName)
{
	//qDebug() << "OnlineLeaderboardWidget::updateName(QUuid " << id << ", QString " << newName << ")";
	/*	User * tmpUser = findUser(id);
	QString name = "";
	if (tmpUser != nullptr)
	{
		name = tmpUser->getName();
		for(QTableWidgetItem *item : tableView->findItems(name,0))
		{
			if(item->data(1) == id && item->text() == name)
			{
				tableView->item(item->row(),item->column())->setText(newName);
				return;
			}
		}
	}
	if (localUser->getId() == id)
	{
		localUser->setName(newName);
		lname->setText(newName);
	}
	lay->update();*/


	model->update();
	if (localUser->getId() == id)
	{
		//localUser->setName(newName);
		lname->setText(newName);
	}
	//lname->setText(newName);
}

void OnlineLeaderboardWidget::updatePlayerInPower(QUuid id, QString newName){
	model->update();
	if(id == localUser->getId()){
		newName = "YOU!!!";
	}
	lplayerInPower->setText(tr("playing: ") + newName);

}

void OnlineLeaderboardWidget::noPlayerInPower(){
	lplayerInPower->setText(" - ");
}

void OnlineLeaderboardWidget::updateColour(QUuid id, QColor color)
{
	//qDebug() << "OnlineLeaderboardWidget::updateColour(QUuid " << id << ", QColor " << color << ")";
	/*	qDebug()<<"useronlinewidget updateColor to "<<color.name();
	User * tmpUser = findUser(id);
	QString name = "";
	if (tmpUser != nullptr)
	{
		name = tmpUser->getName();
		for(QTableWidgetItem *item : tableView->findItems(name,0))
		{
			if(item->data(1) == id && item->text() == name)
			{
				QBrush brush;
				brush.setColor(color);
				tableView->item(item->row(),item->column())->setForeground(brush);;
				return;
			}
		}
	}
	if (localUser->getId() == id)
	{
		localUser->setColor(color);
		lname->setStyleSheet("color: "+color.name());
	}
	lay->update();*/
	model->update();
}

void OnlineLeaderboardWidget::addUser(User *u)
{
	//qDebug() << "OnlineLeaderboardWidget::addUser(User *" << u->getName() << u->getId() << ")";
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
	connect(u, &User::pointsChanged, this, [=]()->void{model->update();});
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
	if(delegate)
	{
		delegate->setButtonVisible(visible);
	}
	tableView->update();
}
