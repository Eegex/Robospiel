#include "networkmodel.h"
#include "Direction.h"
#include <QJsonDocument>
#include <QTime>


QMap<int,QString> NetworkModel::playerActionStrings;

NetworkModel::NetworkModel()
{
	playerActionStrings.insert(0x0,"none");
	playerActionStrings.insert(0x10,"movement");
	playerActionStrings.insert(0x11,"movePlayerNorth");
	playerActionStrings.insert(0x12,"movePlayerEast");
	playerActionStrings.insert(0x14,"movePlayerSouth");
	playerActionStrings.insert(0x18,"movePlayerWest");
	playerActionStrings.insert(0x20,"playerSwitch");
	playerActionStrings.insert(0x21,"switchPlayerNorth");
	playerActionStrings.insert(0x22,"switchPlayerEast");
	playerActionStrings.insert(0x24,"switchPlayerSouth");
	playerActionStrings.insert(0x28,"switchPlayerWest");
	playerActionStrings.insert(0x40,"bidding");
	playerActionStrings.insert(0x41,"enterBidding");
	playerActionStrings.insert(0x42,"sendBidding");
	playerActionStrings.insert(0x44,"clearBidding");
	playerActionStrings.insert(0x48,"skipTimer");
	playerActionStrings.insert(0x80,"other");
	playerActionStrings.insert(0x81,"giveUp");
	playerActionStrings.insert(0x82,"revert");
	playerActionStrings.insert(0x84,"revertToBeginning");
	playerActionStrings.insert(0x88,"freePlay");
	playerActionStrings.insert(0x100,"user");
	playerActionStrings.insert(0x101,"newUser");
	playerActionStrings.insert(0x102,"changedUsername");
	playerActionStrings.insert(0x104,"changedUserColor");
    playerActionStrings.insert(0x108,"userLeft");
	playerActionStrings.insert(0x200,"update");
	playerActionStrings.insert(0x201,"completeUpdate");
	playerActionStrings.insert(0x202,"boardUpdate");
	playerActionStrings.insert(0x400,"menuAction");
	playerActionStrings.insert(0x401,"editBoard");
	playerActionStrings.insert(0x402,"nextTarget");
	playerActionStrings.insert(0x403,"setIdle");
	playerActionStrings.insert(0x1000,"intern");
	playerActionStrings.insert(0x1001,"syncRandomGenerators");
	playerActionStrings.insert(0x1002,"registerClient");
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
