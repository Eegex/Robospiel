#include "user.h"
#include <QString>
#include <QColor>
#include <QUuid>
#include <QJsonObject>

int User::userCount = 1;

User::User(QString name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
	User::id = QUuid::createUuid();
	if(name=="")
	{
		name = tr("user ")+User::userCount++;
	}
}

//User::User(QString name, QColor color, QUuid uuid, QObject *parent) : QObject(parent), name(name), color(color), id(uuid)
//{
//	if(name=="")
//	{
//		name = tr("user ")+User::userCount++;
//	}
//}

User::User(QObject *parent) : QObject(parent)
{

}

void User::setTimeStamp(unsigned long value)
{
    timeStamp = value;
}

QJsonObject User::toJSON()
{
    QJsonObject json;
    json.insert("name", getName());
    json.insert("color", getColor().name());
    json.insert("id", getId().toString());
    json.insert("bidding", getBidding());
    json.insert("points", getPoints());
    json.insert("hasBid", hasBid);

    QString timeStampString = QString("%1").arg(getTimeStamp());
    json.insert("lastBiddingTime", timeStampString);
    return json;
}

User* User::fromJSON(QJsonObject json)
{
    User* user = new User();
    user->setName(json.value("name").toString());
    user->setColor(QColor(json.value("color").toString()));
    user->id = QUuid(json.value("id").toString());
    user->setBidding(json.value("bidding").toInt());
    user->points = json.value("points").toInt();
    user->timeStamp = json.value("lastBiddingTime").toString().toULong();
    user->hasBid = json.value("hasBid").toBool();
    return user;
}

void User::incrementPoints()
{
    points++;
	emit pointsChanged(id, points);
}

QString User::getName() const
{
	return name;
}
QColor User::getColor() const
{
	return color;
}
QUuid User::getId() const
{
	return id;
}
int User::getBidding() const
{
	return bidding;
}
int User::getPoints() const
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
	if(hasBid)
	{
        timeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
	}
	else
	{
        timeStamp = 0;
	}
	emit biddingChanged(id, b);
}

void User::addPoints(int p)
{
	points += p;
}

unsigned long User::getTimeStamp() const
{
    return timeStamp;
}
