#ifndef BOARDEDITOR_H
#define BOARDEDITOR_H

#include "boardview.h"
#include <QWidget>

class BoardEditor : public BoardView
{
	Q_OBJECT
public:
	BoardEditor(QWidget *parent = nullptr);

protected slots:
	Direction tileBorder(QPoint p);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
};

#endif // BOARDEDITOR_H
