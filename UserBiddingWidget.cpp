#include "UserBiddingWidget.h"
#include "mainwindow.h"

/*!
  \class Widget for offline mode.
    Displays username and points. Provides spinbox for bidding inputs.
  */

/**
 * @brief UserBiddingWidget::UserBiddingWidget
 * @param u
 * @param parent
 */
UserBiddingWidget::UserBiddingWidget(User * u, QWidget *parent) : QWidget(parent)
{
	user = u;
	userLayout->addWidget(labelName, 0, 0); //Left
	userLayout->addWidget(lSpinBox, 0, 1); //Center
	userLayout->addWidget(accept, 0, 2); //Right
	userLayout->addWidget(pointsName, 0, 3); //Far Right (AfD)
	setLayout(userLayout);
	connect(accept,&QPushButton::clicked,this,&UserBiddingWidget::btnPressed); //When player has accepted bid, disable the button
	lSpinBox->setMinimum(MIN_BID);
	lSpinBox->setSingleStep(1);
    qDebug()<<"In Creation of UserBiddingWidget, adding user with bidding " << user->getBidding();
	lSpinBox->setValue(user->getBidding());
	updateLayout();
	qDebug()<<"In Creation of UserBiddingWidget, Name is: "<<labelName<<" points are "<<lSpinBox->value();
}

/*!
 * \brief UserBiddingWidget::deactivateBtn
 * This function deactivates the bid button. If the user hasn't made a bid yet, the timestamp is set.
 * This is needed for the edge case where someone legitimately bids the maximum amount but then has to wait for all the people whose bid was set to the maximum amount since technically
 * those people bid before the legitimate person.
 */
void UserBiddingWidget::deactivateBtn()
{
	if(!user->getHasBid())
	{
		user->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
	}
	qDebug()<<"Called Function Deactivate Button in UserBiddingWidget";
	accept->setEnabled(false);
}

/*!
 * \brief UserBiddingWidget::activateBtn enables input spinbox and accept button
 */
void UserBiddingWidget::activateBtn(){
	qDebug()<<"Called Function Activate Button in UserBiddingWidget";
	accept->setEnabled(true);
	lSpinBox->setEnabled(true);
}

/*!
 * \brief UserBiddingWidget::updateLayout
 * This function updates all the layout components associated with a user.
 */
void UserBiddingWidget::updateLayout()
{
	labelName->setText(user->getName());
	labelName->setStyleSheet("color: "+user->getColor().name());
	pointsName->setText("Points: "+QString::number(user->getPoints()));
	lSpinBox->setMaximum(user->getBidding());
	if(!user->getHasBid())
	{
		accept->setText(BID_BTN_TEXT);
	}
	else
	{
		accept->setText("Bid: "+QString::number(user->getBidding()));
	}
}

/*!
 * \brief UserBiddingWidget::updateBidding updates the spin box which displays the bidding
 * \param bidding
 */
void UserBiddingWidget::updateBidding(int bidding){
	lSpinBox->setValue(bidding);
	updateLayout();
}

/*!
 * \brief UserBiddingWidget::updateName updates the username in the GUI
 * \param name
 */
void UserBiddingWidget::updateName(QString name){
	labelName->setText(name);
	updateLayout();
}

/*!
 * \brief UserBiddingWidget::updateColour updates the user color in the GUI
 * \param colour
 */
void UserBiddingWidget::updateColour(QColor colour){
	labelName->setStyleSheet("color: "+colour.name());
	updateLayout();
}

/*!
 * \brief UserBiddingWidget::btnPressed emits biddingChanged, when the bidding is valid.
 */
void UserBiddingWidget::btnPressed()
{
	if(lSpinBox->value() > MIN_BID){
		emit biddingChanged(lSpinBox->value(), QDateTime::currentMSecsSinceEpoch(), user->getId());
		qDebug()<<"Player "<<user->getName()<<" changed their bidding to: "<<lSpinBox->value();
	}else{
		qDebug()<<"Someone has tried to bid No Bid";
	}
}

User* UserBiddingWidget::getUser()
{
	return user;
}

QUuid UserBiddingWidget::getId()
{
	return user->getId();
}
