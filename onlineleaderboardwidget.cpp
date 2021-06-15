#include "onlineleaderboardwidget.h"
#include <QDebug>
#include <QBrush>

OnlineLeaderboardWidget::OnlineLeaderboardWidget()
{

}

void OnlineLeaderboardWidget::initialize()
{
	bidBtn->hide();
	lname->hide();
	biddingBox->hide();
	tableView->hide();
	setOnlineWidget();
	setLayout(lay);

	tableView->setModel(model);
	biddingBox->setSpecialValueText(tr("No Bid"));
	connect(bidBtn,&QPushButton::clicked, this, &OnlineLeaderboardWidget::btnPressed);
    connect(biddingBox,&SpinBox::returnPressed, this,  &OnlineLeaderboardWidget::btnPressed);
	//connect(this, &OnlineLeaderboardWidget::updateLayout, tableView, &QTableWidget::clearContents);
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
	lay->addWidget(lname);
	lay->addWidget(biddingBox);
	lay->addWidget(bidBtn);
	lay->addWidget(tableView);
	lname->setText(localUser->getName());
	lname->show();
	tableView->show();
	biddingBox->show();
	bidBtn->show();
	lay->update();
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
	biddingBox->setMaximum(userBidding = biddingBox->value());
	bidBtn->setText(tr("Bid: ")+QString::number(userBidding));
	biddingBox->setFocus();
	biddingBox->selectAll();
	qDebug() << "Player changed their bidding to: " << userBidding;
	emit biddingAccepted(localUser->getId(), userBidding);
}

void OnlineLeaderboardWidget::updateBidding(QUuid id, int bidding)
{
	qDebug() << "OnlineLeaderboardWidget::updateBidding(QUuid " << id << ", int " << bidding << ")";
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
}

void OnlineLeaderboardWidget::updateName(QUuid id, QString newName)
{
	qDebug() << "OnlineLeaderboardWidget::updateName(QUuid " << id << ", QString " << newName << ")";
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
}

void OnlineLeaderboardWidget::updateColour(QUuid id, QColor color)
{
	qDebug() << "OnlineLeaderboardWidget::updateColour(QUuid " << id << ", QColor " << color << ")";
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
	qDebug() << "OnlineLeaderboardWidget::addUser(User *" << u->getName() << u->getId() << ")";
	connect(u,&User::biddingChanged,this,[&](QUuid id, int bid)
	{
		model->update();
		if(id == localUser->getId())
		{
			biddingBox->setMaximum(std::min(bid,User::maxBid));
			if(!localUser->getHasBid())
			{
				bidBtn->setText(BID_BTN_TEXT);
				biddingBox->setValue(1);
			}
		}
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
}

void OnlineLeaderboardWidget::updateAllUsers()
{
	model->updateUsers();
}
