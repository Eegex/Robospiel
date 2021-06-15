#include "spinbox.h"

#include <QEvent>
#include <QKeyEvent>

SpinBox::SpinBox(QWidget* parent):QSpinBox(parent)
{

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


