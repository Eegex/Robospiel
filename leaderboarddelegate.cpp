#include "leaderboarddelegate.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPalette>

LeaderboardDelegate::LeaderboardDelegate(QSize size,QWidget * parent) : QStyledItemDelegate(parent)
{
	customSizeHint = size;
}

void LeaderboardDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(buttonVisible && index.column() == 0)
	{
		painter->save();
		QPainterPath path;
		painter->setPen(QPen(QColor(0xAAAAAA),1));
		path.addRoundedRect(option.rect.adjusted(5,5,-10,-10),2.5,2.5);
		painter->fillPath(path,QApplication::palette().color(QPalette::ColorRole::Button));
		//muss besser gehen, auch ohne wirklich einen Button zu erzeugen
//		if(index.row() == clicked)
//		{
//		}
//		else if(index.row() == hovered)
//		{
//			painter->fillPath(path,QApplication::palette().color(QPalette::ColorRole::Highlight));
//		}
//		else
//		{
			painter->drawPath(path);
//		}
		painter->restore();
	}
	QStyledItemDelegate::paint(painter,option,index);
}

QSize LeaderboardDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(index.column())
	{
		return QStyledItemDelegate::sizeHint(option, index);
	}
	else
	{
		return customSizeHint;
	}
}

void LeaderboardDelegate::setButtonVisible(bool visible)
{
	buttonVisible = visible;
}

void LeaderboardDelegate::updateSizeHint(int logicalIndex, int /*oldSize*/, int newSize)
{
	if(!logicalIndex)
	{
		customSizeHint.setWidth(newSize);
	}
}

void LeaderboardDelegate::userHovered(const QModelIndex & index)
{
	if(!index.column())
	{
		hovered = index.row();
	}
}

void LeaderboardDelegate::userClicked(const QModelIndex & index)
{
	if(!index.column())
	{
		clicked = index.row();
		if(buttonVisible)
		{
			QModelIndex copy = index;
			emit userBtnClicked(copy);
		}
	}
}
