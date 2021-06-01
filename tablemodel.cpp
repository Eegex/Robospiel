#include "tablemodel.h"

TableModel::TableModel(QObject *parent):QAbstractTableModel(parent)
{

}

void TableModel::update()
{
	emit dataChanged(index(0,0),index(rowCount(QModelIndex()),columnCount(QModelIndex())));
}

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
	if(user)
	{
		return user->size();
	}
	return 0;
}

int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 3;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
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
					return user->at(index.row())->getName();
				case 1:
					if(user->at(index.row())->getHasBid())
					{
						return user->at(index.row())->getBidding();
					}
					return "-";
				case 2:
					return user->at(index.row())->getPoints();
				}
			}
			else if(role == Qt::ForegroundRole)
			{
				if(index.column() == 0)
				{
					return user->at(index.row())->getColor();
				}
			}
		}
	}
	return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Horizontal)
	{
		QStringList header = {tr("Names"),tr("Bidding"),tr("Points")};
		return header.at(section);
	}
	else
	{
		return QVariant();
	}
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

void TableModel::sort(int column, Qt::SortOrder order)
{
	if(!user)
	{
		return;
	}
	emit layoutAboutToBeChanged();
	bool n = order == Qt::SortOrder::AscendingOrder;
	switch(column)
	{
	case 0:
	{
		std::sort(user->begin(),user->end(),[&](const User * a, const User * b){ return n?(a->getName() > b->getName()):(a->getName() < b->getName()); });
		break;
	}
	case 1:
	{
		std::sort(user->begin(),user->end(),[&](const User * a, const User *b){ return n?a < b:b < a; });
		break;
	}
	case 2:
	{
		std::sort(user->begin(),user->end(),[&](const User * a, const User * b){ return n?(a->getPoints() > b->getPoints()):(a->getPoints() < b->getPoints()); });
		break;
	}
	default:
		return;
	}
	emit layoutChanged();
}

void TableModel::addUser(User * newUser)
{
	if(!user)
	{
		return;
	}
	user->append(newUser);
	emit layoutChanged();
}

User * TableModel::findUser(QUuid id)
{
	for(User * u:*qAsConst(user))
	{
		if(u->getId() == id)
		{
			return u;
		}
	}
	Q_ASSERT_X(false,"TableModel::findUser(QUuid id)","User not found");
	return nullptr;
}

void TableModel::setUser(QVector<User *> * newUsers)
{
	user = newUsers;
	emit layoutChanged();
}

