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
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addUserBtn = new QPushButton("Spieler hinzufügen", this); //Translate-Funktion
    QPushButton * addColourBtn = new QPushButton("Farbe hinzufügen", this);
    QColor * UserColor = new QColor();
    QColorDialog * userColourPicker = new QColorDialog();
    QLineEdit * userNamePicker = new QLineEdit();
    unsigned int numOfUsers = 0;

public slots:
    void addUser();
    void addColour();
    void resetButtonColour();
signals:
    void userAdded(struct UserData * newUser); //Argument: Nutzer
};
#endif // USERCREATIONWIDGET_H
