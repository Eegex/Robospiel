#include "client.h"
#include "gamecontroll.h"
#include "server.h"
#include "user.h"
#include <QString>
#include <QColor>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

int User::userCount = 1;

const int User::maxBid = INT32_MAX;

/*!
 * \brief User::User Creates a new User with new Uuid
 * \param name when the name is "", a default name is generated
 * \param color
 * \param parent
 */
User::User(QString name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
	User::id = QUuid::createUuid();
	if(name=="")
	{
        name = tr("user ")+QString::number(User::userCount++);
	}
    User::bidding = User::maxBid; //This should fix the sort function
    User::hasBid = false;
}

User::~User()
{
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

bool User::getHasBid() const
{
	return hasBid;
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
    json.insert("hasVoted", hasVoted);
	json.insert("votekickCount", votekickCount);

	QJsonArray mapUuids;
	QJsonArray mapValues;
	for(QUuid key : votekickMap.keys())
	{
		mapUuids.append(key.toString());
		mapValues.append(votekickMap.value(key));
	}
	json.insert("votekickMapKeys", mapUuids);
	json.insert("votekickMapValues", mapValues);


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
    user->hasVoted = json.value("hasVoted").toBool();

	user->votekickCount = json.value("votekickCount").toInt();
	QJsonArray mapUuids = json.value("votekickMapKeys").toArray();
	QJsonArray mapValues = json.value("votekickMapValues").toArray();
	Q_ASSERT_X(mapUuids.size()==mapValues.size(), "User::fromJson", "Number of keys and values differ!");
	for(int i=0; i<mapUuids.size(); i++)
	{
		user->votekickMap.insert(QUuid(mapUuids.at(i).toString()), mapValues.at(i).toBool());
	}

	return user;
}

void User::incrementPoints()
{
	points++;
	emit pointsChanged(id, points);
}

void User::resetPoints()
{
    points=0;
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

/*!
 * \brief User::setBidding
 * \param b = INT32_MAX to reset
 *
 */
void User::setBidding(int b)
{
	bidding = b;
	hasBid = 1 < b && b < maxBid;
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

unsigned long User::getTimeStamp() const
{
	return timeStamp;
}

bool User::operator<(const User & u) const
{
	return this < &u;
}

bool User::operator<(const User * u) const
{
	if(hasBid && u->hasBid)
	{
		return (getBidding() == u->getBidding() && getTimeStamp() < u->getTimeStamp()) || getBidding() < u->getBidding();
	}
	return hasBid || !u->hasBid;
}

void User::setHasVoted(bool newValue)
{
    hasVoted = newValue;
}

bool User::getHasVoted()
{
    return hasVoted;
}


void User::setVotekick(QUuid toUser, bool boolean)
{
	votekickMap.insert(toUser, boolean);
}

/**
 * @brief User::getVotekick
 * @param toUser
 * @return returns true if the User has already voted to kick toUser
 */
bool User::getVotekick(QUuid toUser)
{
	return votekickMap.value(toUser, false);
}

void User::resetVotekick()
{
	for(QUuid key:votekickMap.keys())
	{
		votekickMap.insert(key, false);
	}
	votekickCount = 0;
}

int User::getVotekickCount()
{
	return votekickCount;
}

void User::receiveVotekick()
{
	Q_ASSERT_X(Client::isActive()||Server::isActive(), "User::receiveVotekick()", "function should only be called, when online mode");
	votekickCount++;
	if(votekickCount>=GameControll::getUsers()->size()-1 && this==GameControll::getLocalUser()) //required number of votes: everyone, but oneself
	{
		if(!GameControll::initiateServerSwitch(false))
		{
			GameControll::requestDisconnect();
		}
	}
}

void User::votekickInfo()
{
	qDebug() << "Votekickinfo for " << name;
	for(auto key : votekickMap.keys())
	{
		qDebug() << "  " <<key << votekickMap.value(key);
	}
	qDebug();
}
