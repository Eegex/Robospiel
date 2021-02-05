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

void UserCreationWidget::addUser()
{
    qDebug()<<"Addition Request of user with name "<<userNamePicker->text()<<"and colour "<<userColourPicker->selectedColor().name();
    User* user = new User(userNamePicker->text()!=nullptr?userNamePicker->text():DEFAULTUSERNAME, userColourPicker->selectedColor());
    qDebug()<<"Current Colour"<<userColourPicker->selectedColor().name();
    delete userColourPicker;
    userColourPicker = new QColorDialog;

    GameControll::triggerActionsWithData(PlayerAction::newUser, user->toJSON());
    connect(userColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: "+colour.name());
    });
}

void UserCreationWidget::resetFields(){
    addColourBtn->setStyleSheet("background-color: 0xFFFFFF");
    userNamePicker->setText("");
}
void UserCreationWidget::addColour(){userColourPicker->show();}
QPushButton * UserCreationWidget::getAddBtn(){return addUserBtn;}
QColorDialog * UserCreationWidget::getColorDialog(){return userColourPicker;}
QLineEdit * UserCreationWidget::getLineEdit(){return userNamePicker;}
