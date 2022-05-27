#include "buttondelegate.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QColor>
#include <QDebug>
#include <QPalette>

ButtonDelegate::ButtonDelegate(QWidget* parent) : QStyledItemDelegate(parent)
{

}

void ButtonDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QPoint p(index.row(), index.column());
	if(hasButton(p) || delay==p)
	{
		QRect r(option.rect);
		r.adjust(smaller,smaller,-smaller,-smaller);
		QPixmap px(r.size());
		px.fill();
		QPushButton btn(index.data().toString());
		btn.setDisabled(buttonDisabled(p));
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

QSize ButtonDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QPoint p(index.row(), index.column());
	if(!getCustomSizeHint(p).isNull())
	{
		return getCustomSizeHint(p);
	}
	else
	{
		return QStyledItemDelegate::sizeHint(option, index);
	}
}

void ButtonDelegate::handleHover(const QModelIndex & index)
{
	QPoint p(index.row(), index.column());
	if(hasButton(p))
	{
		hovered = p;
		QModelIndex copy = index;
		emit buttonHovered(copy);
	}
}

void ButtonDelegate::handleClick(const QModelIndex & index)
{
	QPoint p(index.row(), index.column());
	if(hasButton(p))
	{
		clicked = p;
		delay = p;
		QModelIndex copy = index;
		emit buttonClicked(copy);
	}
}

void ButtonDelegate::resetState()
{
	clicked = QPoint(-1,-1);
	delay = QPoint(-1,-1);
	hovered = QPoint(-1,-1);
}
