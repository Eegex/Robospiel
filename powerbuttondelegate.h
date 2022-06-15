#ifndef POWERBUTTONDELEGATE_H
#define POWERBUTTONDELEGATE_H

#include "buttondelegate.h"
#include <QWidget>

class PowerButtonDelegate : public ButtonDelegate
{
	Q_OBJECT
public:
	PowerButtonDelegate(QSize size, QWidget * parent = nullptr);
	void setButtonVisible(bool visible);
	void updateSizeHint(int logicalIndex, int oldSize, int newSize);

private:
	int hasButton(QPoint p) const;
	bool isButtonDisabled(QPoint p) const;
	QSize getCustomSizeHint(QPoint p) const;

	QSize customSizeHint = {80,20};
	bool buttonVisible = false;

};

#endif // POWERBUTTONDELEGATE_H
