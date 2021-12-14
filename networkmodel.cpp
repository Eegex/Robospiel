#include "networkmodel.h"
#include "Direction.h"
#include <QJsonDocument>
#include <QTime>


QMap<int,QString> NetworkModel::playerActionStrings;

NetworkModel::NetworkModel()
{
	playerActionStrings.insert(PlayerAction::none,"none");
	playerActionStrings.insert(PlayerAction::movement,"movement");
	playerActionStrings.insert(PlayerAction::movePlayerNorth,"movePlayerNorth");
	playerActionStrings.insert(PlayerAction::movePlayerEast,"movePlayerEast");
	playerActionStrings.insert(PlayerAction::movePlayerSouth,"movePlayerSouth");
	playerActionStrings.insert(PlayerAction::movePlayerWest,"movePlayerWest");
	playerActionStrings.insert(PlayerAction::playerSwitch,"playerSwitch");
	playerActionStrings.insert(PlayerAction::switchPlayerNorth,"switchPlayerNorth");
	playerActionStrings.insert(PlayerAction::switchPlayerEast,"switchPlayerEast");
	playerActionStrings.insert(PlayerAction::switchPlayerSouth,"switchPlayerSouth");
	playerActionStrings.insert(PlayerAction::switchPlayerWest,"switchPlayerWest");
	playerActionStrings.insert(PlayerAction::bidding,"bidding");
	playerActionStrings.insert(PlayerAction::enterBidding,"enterBidding");
	playerActionStrings.insert(PlayerAction::sendBidding,"sendBidding");
	playerActionStrings.insert(PlayerAction::clearBidding,"clearBidding");
	playerActionStrings.insert(PlayerAction::vote,"vote");
	playerActionStrings.insert(PlayerAction::other,"other");
	playerActionStrings.insert(PlayerAction::giveUp,"giveUp");
	playerActionStrings.insert(PlayerAction::revert,"revert");
	playerActionStrings.insert(PlayerAction::revertToBeginning,"revertToBeginning");
	playerActionStrings.insert(PlayerAction::freePlay,"freePlay");
	playerActionStrings.insert(PlayerAction::user,"user");
	playerActionStrings.insert(PlayerAction::newUser,"newUser");
	playerActionStrings.insert(PlayerAction::changedUsername,"changedUsername");
	playerActionStrings.insert(PlayerAction::changedUserColor,"changedUserColor");
	playerActionStrings.insert(PlayerAction::changedTimerTime,"changedTimerTime");
	playerActionStrings.insert(PlayerAction::userLeft,"userLeft");
	playerActionStrings.insert(PlayerAction::update,"update");
	playerActionStrings.insert(PlayerAction::completeUpdate,"completeUpdate");
	playerActionStrings.insert(PlayerAction::boardUpdate,"boardUpdate");
	playerActionStrings.insert(PlayerAction::menuAction,"menuAction");
	playerActionStrings.insert(PlayerAction::editBoard,"editBoard");
	playerActionStrings.insert(PlayerAction::blockBoard,"blockBoard");
	playerActionStrings.insert(PlayerAction::nextTarget,"nextTarget");
	playerActionStrings.insert(PlayerAction::setIdle,"setIdle");
	playerActionStrings.insert(PlayerAction::intern,"intern");
	playerActionStrings.insert(PlayerAction::syncRandomGenerators,"syncRandomGenerators");
	playerActionStrings.insert(PlayerAction::registerClient,"registerClient");
	playerActionStrings.insert(PlayerAction::changeActiveUser,"changeActiveUser");
	columns << "Zeitpunkt" << "Sender" << "Nutzer" << "PlayerAction" << "Daten";
}

void NetworkModel::addTransmission(QJsonObject & transmission)
{
	emit layoutAboutToBeChanged();
	transmission.insert("Timestamp",QTime::currentTime().toString("hh:mm:ss.zzz"));
	list << transmission;
	emit layoutChanged();
}

void NetworkModel::clear()
{
	emit layoutAboutToBeChanged();
	list.clear();
	emit layoutChanged();
}

int NetworkModel::rowCount(const QModelIndex & /*parent*/) const
{
	return list.size();
}

int NetworkModel::columnCount(const QModelIndex & /*parent*/) const
{
	return columns.size();
}

QVariant NetworkModel::data(const QModelIndex &index, int role) const
{
	if(index.isValid())
	{
		if(index.row() < rowCount(QModelIndex()) && index.column() < columnCount(QModelIndex()))
		{
			const QJsonObject & object = list.at(index.row());
			if(role == Qt::DisplayRole)
			{
				switch(index.column())
				{
				case 0:
					return object.value("Timestamp");
				case 1:
					return object.value("Client");
				case 2:
				{
					QUuid id;
					if(!object.value("userId").isUndefined())
					{
						id = object.value("userId").toString();
					}
					else if(!object.value("activeUserID").isUndefined())
					{
						id = object.value("activeUserID").toString();
					}
					else if(!object.value("id").isUndefined())
					{
						id = object.value("id").toString();
					}
					if(id.isNull())
					{
						return "Unbekannt";
					}
					for(User * u:*users)
					{
						if(u && u->getId() == id)
						{
							return u->getName();
						}
					}
					return id;
				}
				case 3:
					return playerActionStrings.value(object.value("action").toInt());
				case 4:
					QJsonDocument doc(object);
					return doc.toJson(QJsonDocument::Compact);
				}
			}
			else if(role == Qt::ForegroundRole)
			{
				return QVariant();
			}
			else if(role == Qt::BackgroundRole)
			{
				if(index.column() == 2)
				{
					QUuid id;
					if(!object.value("userId").isUndefined())
					{
						id = object.value("userId").toString();
					}
					else if(!object.value("activeUserID").isUndefined())
					{
						id = object.value("activeUserID").toString();
					}
					else if(!object.value("id").isUndefined())
					{
						id = object.value("id").toString();
					}
					if(!id.isNull())
					{
						for(User * u:*users)
						{
							if(u && u->getId() == id)
							{
								return u->getColor();
							}
						}
					}
				}
				else if(!object.value("bidding").isUndefined() && index.column() == 3)
				{
					return QColor(0,180,255,80);
				}
				if(list.at(index.row()).value("Client").toString("Server") == "Server")
				{
					return QColor(255,125,0,80);
				}
				else
				{
					return QColor(74,127,44,80);
				}
			}
		}
	}
	return QVariant();
}

QVariant NetworkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(orientation == Qt::Horizontal)
		{
			if(section < columns.size())
			{
				return columns.at(section);
			}
		}
		else
		{
			return section;
		}
	}
	return QVariant();
}

void NetworkModel::setUsers(QVector<User *> * value)
{
	users = value;
}
