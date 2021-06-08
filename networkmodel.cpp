#include "networkmodel.h"
#include "Direction.h"
#include <QJsonDocument>


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
	columns << "Zeitpunkt" << "Client" << "PlayerAction" << "Daten";
}

void NetworkModel::addTransmission(QJsonObject & transmission)
{
	emit layoutAboutToBeChanged();
	transmission.insert("Timestamp",QTime::currentTime().toString("hh:MM:ss.zzz"));
	list << transmission;
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
			if(role == Qt::DisplayRole)
			{
				switch(index.column())
				{
				case 0:
					return list.at(index.row()).value("Timestamp");
				case 1:
					return list.at(index.row()).value("Client");
				case 2:
					return playerActionStrings.value(list.at(index.row()).value("action").toInt());
				case 3:
					return list.at(index.row());
				}
			}
			else if(role == Qt::ForegroundRole)
			{
				return QVariant();
				if(index.column() == 0)
				{
					if(list.at(index.row()).value("Client").toString("Server") == "Server")
					{
						return QColor(255,125,0);
					}
					else
					{
						return QColor(74,127,44);
					}
				}
			}
			else if(role == Qt::BackgroundRole)
			{
				if(list.at(index.row()).value("Client").toString("Server") == "Server")
				{
					return QColor(255,125,0);
				}
				else
				{
					return QColor(74,127,44);
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
