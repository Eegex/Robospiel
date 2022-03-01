#include "leaderboarddelegate.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QColor>
#include <QDebug>
#include <QPalette>

LeaderboardDelegate::LeaderboardDelegate(QSize size,QWidget * parent) : QStyledItemDelegate(parent)
{
	customSizeHint = size;
}

void LeaderboardDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(buttonVisible && index.column() == 0)
	{
		QRect r(option.rect);
		r.adjust(smaller,smaller,-smaller,-smaller);
		QPixmap px(r.size());
		px.fill();
		QPushButton btn(index.data().toString());
		btn.setDisabled(clicked);
		btn.setStyleSheet("color:" + index.data(Qt::ForegroundRole).value<QColor>().name());
		btn.setGeometry(r);
		btn.render(&px,QPoint(0,0),QRect({0,0},r.size()));
		painter->drawPixmap(r,px);
	}
	else
	{
		QStyledItemDelegate::paint(painter,option,index);
	}
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
