#include "onlineleaderboardwidget.h"
#include <QDebug>
#include <QBrush>

OnlineLeaderboardWidget::OnlineLeaderboardWidget()
{
}

void OnlineLeaderboardWidget::initialize() {
	bidBtn->hide();
	lname->hide();
	biddingBox->hide();
    tableView->hide();
	setOnlineWidget();
	setLayout(lay);

    tableView->setModel(model);
    tableView->setSortingEnabled(true);
	connect(bidBtn,&QPushButton::clicked, this, &OnlineLeaderboardWidget::btnPressed);
    //connect(this, &OnlineLeaderboardWidget::updateLayout, tableView, &QTableWidget::clearContents);
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

void OnlineLeaderboardWidget::setLocalUser(User *u)
{
	localUser = u;
}

void OnlineLeaderboardWidget::btnPressed()
{
	biddingBox->setMaximum(userBidding = biddingBox->value());
	bidBtn->setText("Bid: "+QString::number(userBidding));
	qDebug()<<"Player changed their bidding to: "<<userBidding;
	emit biddingAccepted(localUser->getId(), userBidding);
}

void OnlineLeaderboardWidget::updateBidding(QUuid id, int bidding)
{
//	qDebug()<<"useronlinewidget updateBidding to "<<bidding;
//	User * tmpUser = findUser(id);
//	QString name = "";
//	if (tmpUser != nullptr)
//	{
//		name = tmpUser->getName();
//		for(QTableWidgetItem *item : tableView->findItems(name,0))
//		{
//			if(item->data(1) == id && item->text() == name)
//			{
//				tableView->item(item->row(),item->column()+1)->setText(QString::number(bidding));
//				return;
//			}
//		}
//	}
//	// userBidding = bidding; notwendig?
//	// emit biddingChangedOnline(userBidding);
}

void OnlineLeaderboardWidget::updateName(QUuid id, QString newName)
{
//	qDebug()<<"useronlinewidget updateName to "<<newName;
//	User * tmpUser = findUser(id);
//	QString name = "";
//	if (tmpUser != nullptr)
//	{
//		name = tmpUser->getName();
//		for(QTableWidgetItem *item : tableView->findItems(name,0))
//		{
//			if(item->data(1) == id && item->text() == name)
//			{
//				tableView->item(item->row(),item->column())->setText(newName);
//				return;
//			}
//		}
//	}
//	if (localUser->getId() == id)
//	{
//		localUser->setName(newName);
//		lname->setText(newName);
//	}
//	lay->update();
}

void OnlineLeaderboardWidget::updateColour(QUuid id, QColor color)
{
//	qDebug()<<"useronlinewidget updateColor to "<<color.name();
//	User * tmpUser = findUser(id);
//	QString name = "";
//	if (tmpUser != nullptr)
//	{
//		name = tmpUser->getName();
//		for(QTableWidgetItem *item : tableView->findItems(name,0))
//		{
//			if(item->data(1) == id && item->text() == name)
//			{
//				QBrush brush;
//				brush.setColor(color);
//				tableView->item(item->row(),item->column())->setForeground(brush);;
//				return;
//			}
//		}
//	}
//	if (localUser->getId() == id)
//	{
//		localUser->setColor(color);
//		lname->setStyleSheet("color: "+color.name());
//	}
//	lay->update();
}

void OnlineLeaderboardWidget::addUser(User *u)
{
    model->addUser(u);
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
	qDebug()<<"Called Function UpdateAllUsers in OnlineLeaderBoardWidget\n";
    model->setUser(*GameControll::getInstance().getUsers());

}

void OnlineLeaderboardWidget::updateLayout()
{
//    int row = 0;
//    int column = 0;
//    for(User* u : users)
//    {
//        // set name column
//        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg((row+2)*(column+2)));
//        newItem->setText(u->getName());
//        newItem->setData(1,u->getId());
//        QBrush brush;
//        brush.setColor(u->getColor());
//        newItem->setForeground(brush);
//        tableView->setItem(row, column, newItem);
//        // set bidding column
//        QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg((row+2)*(column+3)));
//        newItem2->setText(QString::number(userBidding));
//        newItem2->setData(1,u->getId());
//        tableView->setItem(row, column+1, newItem2);
//        // set point column
//        QTableWidgetItem *newItem3 = new QTableWidgetItem(tr("%1").arg((row+2)*(column+4)));
//        newItem3->setText(QString::number(0));
//        newItem3->setData(1,u->getId());
//        tableView->setItem(row, column+2, newItem3);
//        row += 1;
//    }
}

User * OnlineLeaderboardWidget::findUser(QUuid id)
{
//	for(User *u : model)
//	{
//		if(u->getId() == id)
//		{
//			return u;
//		}
//	}
//	return nullptr; //bessere Lösung
}
