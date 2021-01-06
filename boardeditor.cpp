#include "boardeditor.h"

BoardEditor::BoardEditor(QWidget * parent) : BoardView(parent)
{

}

Direction BoardEditor::tileBorder(QPoint p)
{
	p -= QPoint(5,5);
	int x = (p.x()) / tileSize.width();
	int y = (p.y()) / tileSize.height();
	int w = p.x() - x * tileSize.width();
	int h = p.y() - y * tileSize.height();
	if(w < 10)
	{
		return Direction::west;
	}
	else if(w > tileSize.width() - 10)
	{
		return Direction::east;
	}
	else if(h < 10)
	{
		return Direction::north;
	}
	else if(h > tileSize.height() - 10)
	{
		return Direction::south;
	}
	return Direction::none;
}

void BoardEditor::mousePressEvent(QMouseEvent * event)
{
	Tile * t = coordsToTile(event->pos());
	if(t)
	{
		Direction d = tileBorder(event->pos());
		if(d != Direction::none)
		{
			t->setWall(d,!t->getWall(d));
			update();
		}
	}
}

void BoardEditor::mouseMoveEvent(QMouseEvent * event)
{

}
