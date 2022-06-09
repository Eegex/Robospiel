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
	QPoint p(index.column(), index.row());

	//Button
	if(hasButton(p)==2 || delay==p)
	{
		QRect r(option.rect);
		r.adjust(smaller,smaller,-smaller,-smaller);
		QPixmap px(r.size());
		px.fill();
		QPushButton btn(index.data().toString());
		//btn.setDisabled(isButtonDisabled(p));
		btn.setStyleSheet("color:" + index.data(Qt::ForegroundRole).value<QColor>().name());
		btn.setDisabled(isButtonDisabled(p));
		btn.setGeometry(r);
		btn.render(&px,QPoint(0,0),QRect({0,0},r.size()));
		painter->drawPixmap(r,px);
	}
	else
	{
		//only text
		if(hasButton(p)==1)
		{
			QStyledItemDelegate::paint(painter,option,index);
		}
		else{
			//white
			QRect r(option.rect);
			QPixmap px(r.size());
			px.fill();
			painter->drawPixmap(r,px);
		}

	}
}

QSize ButtonDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QPoint p(index.column(), index.row());
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
	QPoint p(index.column(), index.row());
	if(hasButton(p)==2)
	{
		hovered = p;
		QModelIndex copy = index;
		emit buttonHovered(copy);
	}
}

void ButtonDelegate::handleClick(const QModelIndex & index)
{
	QPoint p(index.column(), index.row());
	if(hasButton(p)==2 && !isButtonDisabled(p))
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
