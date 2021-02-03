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
	const int offset = 17;
	if(h > offset / 2 && h < tileSize.height() - offset / 2)
	{
		if(w < offset)
		{
			return Direction::west;
		}
		else if(w > tileSize.width() - offset)
		{
			return Direction::east;
		}
	}
	else if(w > offset / 2 && w < tileSize.width() - offset / 2)
	{
		if(h < offset)
		{
			return Direction::north;
		}
		else if(h > tileSize.height() - offset)
		{
			return Direction::south;
		}
	}
	return Direction::none;
}

QRect BoardEditor::rect(Tile * t)
{
	int x = 5 + t->getPosition().x() * tileSize.width();
	int y = 5 + t->getPosition().y() * tileSize.height();
	QRect r(x,y,tileSize.width(),tileSize.height());
	return r;
}

void BoardEditor::placePawn(PawnWidget * pawn)
{
	PlayerWidget * player = dynamic_cast<PlayerWidget*>(pawn);
	GoalWidget * goal = dynamic_cast<GoalWidget*>(pawn);
	if(player)
	{
		int playerNumber = playerWidgets.indexOf(player);
		board->players.at(playerNumber)->setPlayer(-1);
		Tile * newPlayer = coordsToTile(player->mapToParent(player->rect().center()));
		if(newPlayer && newPlayer->getPlayer() == -1)
		{
			newPlayer->setPlayer(playerNumber);
			board->players[playerNumber] = newPlayer;
			player->move(tileToDesktopCoordinates(newPlayer));
		}
		else
		{
			board->players.remove(playerNumber);
			delete playerWidgets.takeAt(playerNumber);
			for(int i = 0; i < board->players.size();i++)
			{
				board->players.at(i)->setPlayer(i);
				playerWidgets.at(i)->setPlayer(i);
			}
		}
	}
	else if(goal)
	{
		Tile * newGoal = coordsToTile(goal->mapToParent(goal->rect().center()));
		if(newGoal)
		{
			board->goal = newGoal;
		}
		goalwidget->move(tileToDesktopCoordinates(board->goal));
	}
	update();
}

void BoardEditor::mousePressEvent(QMouseEvent * event)
{
	Tile * t = coordsToTile(event->pos());
	if(t)
	{
		Direction d = tileBorder(event->pos());
		QRect r = rect(t) - QMargins(5,5,5,5);
		if(d == Direction::none && r.contains(event->pos()))
		{
			addPlayer(board->addPlayer(t));
		}
		else
		{
			t->setWall(d,!t->getWall(d));
		}
		update();
	}
}

void BoardEditor::mouseMoveEvent(QMouseEvent * event)
{
//	if(event->buttons() == Qt::LeftButton)
//	{
//		Tile * t = coordsToTile(event->pos());
//		if(t)
//		{
//			Direction d = tileBorder(event->pos());
//			if(d != Direction::none && (lastTile != t || lastDirection != d))
//			{
//				t->setWall(d,!t->getWall(d));
//				lastTile = t;
//				lastDirection = d;
//			}
//			update();
//		}
//	}
}

void BoardEditor::mouseReleaseEvent(QMouseEvent * event)
{
	lastTile = nullptr;
	lastDirection = Direction::none;
	BoardView::mouseReleaseEvent(event);
}

void BoardEditor::setBoard(Board * b)
{
	BoardView::setBoard(b);
	goalwidget->setEditable();
	connect(goalwidget,&PawnWidget::placeMe,this,&BoardEditor::placePawn);
}

PlayerWidget * BoardEditor::addPlayer(int i)
{
	PlayerWidget * newPlayer = BoardView::addPlayer(i);
	newPlayer->setEditable();
	connect(newPlayer,&PawnWidget::placeMe,this,&BoardEditor::placePawn);
	return newPlayer;
}
