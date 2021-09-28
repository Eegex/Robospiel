#include "UserCreationWidget.h"
#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QDebug>
#include <QString>
#include "gamecontroll.h"
#include "Direction.h"
#include "user.h"
#define DEFAULTUSERNAME tr("New User")

/*!
    \class Widget to create a new user for the offline mode.
    Contains a line edit for the user name and can open a colorpicker.
*/
/*!
 * @brief UserCreationWidget::UserCreationWidget
 * @param parent parent widget
 */
UserCreationWidget::UserCreationWidget(QWidget *parent) : QWidget(parent)
{
	connect(addColourBtn,&QPushButton::clicked,this,&UserCreationWidget::addColour);
	userColourPicker->hide(); //Hide Colour Picker as it's not needed yet
	lay->addWidget(userNamePicker, 0, 0); //Left
	lay->addWidget(addColourBtn, 0, 1); //Center
	lay->addWidget(addUserBtn, 0, 2); //Right
	setFocus();
	setFocusPolicy(Qt::TabFocus);
	setTabOrder(userNamePicker, addColourBtn);
	setTabOrder(addColourBtn, addUserBtn);

	connect(userColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
	{
	   addColourBtn->setStyleSheet("background-color: "+colour.name());
	});
	connect(addUserBtn, &QPushButton::clicked, this, &UserCreationWidget::addUser);
	connect(addUserBtn, &QPushButton::clicked, this, &UserCreationWidget::resetFields);
	setLayout(lay);
}

/*!
 * \brief UserCreationWidget::addUser
 * This function adds a new user to the backend and calls TriggerActionWithData
 */
void UserCreationWidget::addUser()
{
	qDebug()<<"Addition Request of user with name "<<userNamePicker->text()<<"and colour "<<userColourPicker->selectedColor().name();
    qDebug()<<"Current Colour"<<userColourPicker->selectedColor().name();
	User* user = new User(userNamePicker->text()!=nullptr?userNamePicker->text():DEFAULTUSERNAME, userColourPicker->selectedColor());
    GameControll::triggerActionWithData(PlayerAction::newUser, user->toJSON());

	delete userColourPicker;
	userColourPicker = new QColorDialog;
    connect(userColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: " + colour.name());
    });
}

/*!
 * @brief UserCreationWidget::resetFields resets the selected color and the name field
 */
void UserCreationWidget::resetFields()
{
	addColourBtn->setStyleSheet("background-color: 0xFFFFFF");
	userNamePicker->setText("");
}

/*!
 * @brief UserCreationWidget::addColour displays a separate window with the colorpicker
 */
void UserCreationWidget::addColour()
{
    userColourPicker->show();
}
