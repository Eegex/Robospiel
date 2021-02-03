#ifndef BOARDEDITOR_H
#define BOARDEDITOR_H

#include "boardview.h"
#include <QWidget>

class BoardEditor : public BoardView
{
	Q_OBJECT
public:
	BoardEditor(QWidget *parent = nullptr);
	void setBoard(Board * b);

protected slots:
	Direction tileBorder(QPoint p);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
protected:
	PlayerWidget * addPlayer(int i);
private slots:
	void placePawn(PawnWidget * pawn);
private:
	QRect rect(Tile * t);
	Tile * lastTile = nullptr;
	Direction lastDirection = Direction::none;
};

#endif // BOARDEDITOR_H
