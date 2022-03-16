#include "spinbox.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>

SpinBox::SpinBox(QWidget* parent):QSpinBox(parent)
{

}

void SpinBox::focusInEvent(QFocusEvent * event)
{
	QSpinBox::focusInEvent(event);
	if(selectAllOnFocus)
	{
		selectAll();
		/* selectAll() funktioniert angeblich nicht. Es verhält sich merkwürdig (Code dadrunter wird nicht direkt ausgeführt) oder es ist ein Problem mit qDebug() und dem Debugger, der sich auch komisch verhalten hat...
		 * Insgesamt läuft es aber. Wenn es nochmal Probleme gibt, nochmal mehr hiervon ausprobieren: https://stackoverflow.com/questions/1891744/pyqt4-qspinbox-selectall-not-working-as-expected
		 * QTimer::singleShot(0, this, &QSpinBox::selectAll); hat irgendwie nichts geändert...
		 */
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

bool SpinBox::getSelectAllOnFocus() const
{
	return selectAllOnFocus;
}

void SpinBox::setSelectAllOnFocus(bool selectAllOnFocus)
{
	this->selectAllOnFocus = selectAllOnFocus;
}


