#ifndef USERCREATIONWIDGET_H
#define USERCREATIONWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QLineEdit>
#include "Direction.h"

class UserCreationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserCreationWidget(QWidget *parent = nullptr);

private:
    QPushButton * addUserBtn = new QPushButton("Spieler hinzufügen", this); //Translate-Funktion
    QColorDialog * userColourPicker = new QColorDialog();
    QLineEdit * userNamePicker = new QLineEdit();
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addColourBtn = new QPushButton("Farbe hinzufügen", this);
    QColor * UserColor = new QColor();
    unsigned int numOfUsers = 0;

public slots:
    void addUser();
    void addColour();
    void resetFields();
    QPushButton * getAddBtn();
    QColorDialog * getColorDialog();
    QLineEdit * getLineEdit();
signals:
    void userAdded(struct UserData * newUser); //Argument: Nutzer
};
#endif // USERCREATIONWIDGET_H
