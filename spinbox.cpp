#include "spinbox.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

SpinBox::SpinBox(QWidget* parent):QSpinBox(parent)
{

}

void SpinBox::focusInEvent(QFocusEvent * event)
{
	QSpinBox::focusInEvent(event);
	if(selectAllOnFocus)
	{
		selectAll(); //BUG: does not work
	}
}

void SpinBox::keyPressEvent(QKeyEvent* event)
{
	if(event->key()==Qt::Key_Enter||event->key()==Qt::Key_Return)
	{
		event->accept();
		emit returnPressed();
	}
	else
	{
		QSpinBox::keyPressEvent(event);
	}
}

void SpinBox::setSelectAllOnFocus(bool selectAllOnFocus)
{
	this->selectAllOnFocus = selectAllOnFocus;
}


