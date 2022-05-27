#include "powerbuttondelegate.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QColor>
#include <QDebug>
#include <QPalette>

PowerButtonDelegate::PowerButtonDelegate(QSize size,QWidget * parent) : ButtonDelegate(parent)
{
	customSizeHint = size;
}

QSize PowerButtonDelegate::getCustomSizeHint(QPoint p) const
{
	if(p.y()==0)
	{
		return customSizeHint;
	}
	else
	{
		return {0,0};
	}
}

void PowerButtonDelegate::setButtonVisible(bool visible)
{
	buttonVisible = visible;
}

void PowerButtonDelegate::updateSizeHint(int logicalIndex, int /*oldSize*/, int newSize)
{
	if(logicalIndex==0)
	{
		customSizeHint.setWidth(newSize);
	}
}

bool PowerButtonDelegate::hasButton(QPoint p) const
{
	return buttonVisible && p.y()==0;
}

bool PowerButtonDelegate::buttonDisabled(QPoint p) const
{
	return p.x()==clicked.x();
}


