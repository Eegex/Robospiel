#include "tablemodel.h"
#include "gamecontroll.h"

TableModel::TableModel(QObject *parent):QAbstractTableModel(parent)
{

}

/*!
 * \brief TableModel::update forces every View to update its data but not to display new elements
 */
void TableModel::update()
{
	emit dataChanged(index(0,0),index(rowCount(QModelIndex()),columnCount(QModelIndex())));
}

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
	return GameControll::getUsers()->size();
}

/*!
 * \brief TableModel::columnCount returns the amount of colums for every point in the table
 * \return 3
 */
int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
	//Name, Bidding, Points, Votekick-Button
	return 4;
}

/*!
 * \brief TableModel::data If the given index is valid, it returns either the corresponding data (name, bidding or points) if
 * the role is Qt::DisplayRole or it returns the color of the given row if the first column is given
 * \param index row and columnnumber
 * \param role DisplayRole or ForegroundRole
 * \return corresponding data
 */
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
					return GameControll::getUsers()->at(index.row())->getName();
				case 1:
					if(GameControll::getUsers()->at(index.row())->getHasBid())
					{
						return GameControll::getUsers()->at(index.row())->getBidding();
					}
					return "-";
				case 2:
					return GameControll::getUsers()->at(index.row())->getPoints();
				case 3:
					if(index.column()==3 && (GameControll::getUserIndexById(GameControll::getLocalUser()->getId())!=index.row()))
					{
						return tr("Votekick") + " " + GameControll::getUsers()->at(index.row())->getName();
					}
					else
					{
						return QString::number(GameControll::getUsers()->at(index.row())->getVotekickCount()) + "/"+ QString::number(GameControll::getUsers()->size()-1);
					}

				}
			}
			else if(role == Qt::ForegroundRole)
			{
				if(index.column() == 0)
				{
					return GameControll::getUsers()->at(index.row())->getColor();
				}
			}
			else if(role == Qt::TextAlignmentRole)
			{
				return Qt::AlignCenter;
			}
		}
	}
	return QVariant();
}

/*!
 * \brief TableModel::headerData only available if role is Qt::isplayRole and orientation is Qt::Horizontal
 * \param section
 * \param orientation
 * \param role
 * \returns the given section of the header (Names, Bidding or Points)
 */
QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Horizontal)
	{
		QStringList header = {tr("Names"),tr("Bidding"),tr("Points"), tr("Votekick")};
		return header.at(section);
	}
	else
	{
		return QVariant();
	}
}

/*!
 * \brief TableModel::flags returns if items of the table can be selected or if the user can interact with it
 * \return Qt::ItemIsSelectable or Qt::ItemIsEnabled
 */
Qt::ItemFlags TableModel::flags(const QModelIndex & /*&index*/) const
{
	return Qt::ItemIsEnabled;
}

/*!
 * \brief TableModel::updateUsers forces every View to update its data and to display new elements
 */
void TableModel::updateUsers()
{
	emit layoutChanged();
}

/*!
 * \brief TableModel::findUser returns user with the given id if existing in GameControll
 * \param id of the user
 * \return user of given id
 */
User * TableModel::findUser(QUuid id)
{
	for(User * u: *GameControll::getUsers())
	{
		if(u->getId() == id)
		{
			return u;
		}
	}
	Q_ASSERT_X(false,"TableModel::findUser(QUuid id)","User not found");
	return nullptr;
}
