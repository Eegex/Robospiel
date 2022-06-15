#ifndef VOTEKICKDELEGATE_H
#define VOTEKICKDELEGATE_H

#include "buttondelegate.h"

class VotekickDelegate : public ButtonDelegate
{
	Q_OBJECT
public:
	VotekickDelegate(QSize size, QWidget * parent = nullptr);
	void updateSizeHint(int logicalIndex, int oldSize, int newSize);
private:
	int hasButton(QPoint p) const;
	bool isButtonDisabled(QPoint p) const;
	QSize getCustomSizeHint(QPoint p) const;

	QSize customSizeHint = {80,20};
};

#endif // VOTEKICKDELEGATE_H
