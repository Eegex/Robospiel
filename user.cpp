#include "user.h"
#include <QString>
#include <QColor>
#include <QUuid>

int User::userCount = 1;

User::User(QString name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
    User::id = QUuid::createUuid();
    if(name=="")
    {
        name = tr("user ")+User::userCount++;
    }
}

User::User(QString name, QColor color, QUuid uuid, QObject *parent) : QObject(parent), name(name), color(color), id(uuid)
{
    if(name=="")
    {
        name = tr("user ")+User::userCount++;
    }
}

QString User::getName()
{
	return name;
}
QColor User::getColor()
{
	return color;
}
QUuid User::getId()
{
    return id;
}
int User::getBidding()
{
    return bidding;
}
int User::getPoints()
{
	return points;
}

void User::setName(QString n)
{
	name = n;
}
void User::setColor(QColor c)
{
	color = c;
}
void User::setBidding(int b)
{
    bidding = b;
}
void User::addPoints(int p)
{
	points += p;
}
