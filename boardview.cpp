#include "boardview.h"

BoardView::BoardView(QWidget *parent) : QWidget(parent)
{
	fillCache(QSize(50,50));
}

void BoardView::setBoard(Board * b)
{
	this->board = b;
	update();
}

void BoardView::setDebugOutputEnabled(bool set)
{
	showDebugOutput = set;
}

QSize BoardView::sizeHint() const
{
	return board->getSize() * 50;
}

void BoardView::fillCache(QSize tileSize)
{
	cache.clear();
	QPen thin(QColor(50,50,50),1);
	QPen fat(QColor(120,120,120),10);
	QPainter painter;
	QRect tileRect(0,0,tileSize.width(),tileSize.height());
	QPixmap tile(tileSize);
	for(int i = 0;i < 16; i++)
	{
		painter.begin(&tile);
		painter.setPen(thin);
		painter.setBrush(QColor(20,20,20));
		painter.drawRect(tileRect);
		painter.setPen(fat);
		if(i & static_cast<int>(Direction::north))
		{
			painter.drawLine(tileRect.topLeft(),tileRect.topRight());
		}
		if(i & static_cast<int>(Direction::east))
		{
			painter.drawLine(tileRect.topRight(),tileRect.bottomRight());
		}
		if(i & static_cast<int>(Direction::south))
		{
			painter.drawLine(tileRect.bottomRight(),tileRect.bottomLeft());
		}
		if(i & static_cast<int>(Direction::west))
		{
			painter.drawLine(tileRect.bottomLeft(),tileRect.topLeft());
		}
		painter.end();
		cache.insert(QString::number(i,2),tile);
	}
}

void BoardView::paintEvent(QPaintEvent * event)
{
	QPainter painter;
	QPixmap tile;
	painter.begin(this);
	painter.setPen(QColor(255,0,255));
	for(int x = 0; x < board->getSize().width(); x++)
	{
		for(int y = 0; y < board->getSize().height(); y++)
		{
			int key = (board->getTile(x,y)->getWall(Direction::north)?static_cast<int>(Direction::north):0) |
					  (board->getTile(x,y)->getWall(Direction::east)?static_cast<int>(Direction::east):0) |
					  (board->getTile(x,y)->getWall(Direction::south)?static_cast<int>(Direction::south):0) |
					  (board->getTile(x,y)->getWall(Direction::west)?static_cast<int>(Direction::west):0);
			if(cache.find(QString::number(key,2),&tile))
			{
				painter.drawPixmap(x*tile.width(),y*tile.height(),tile);
			}
			if(showDebugOutput)
			{
				int playerNumber = board->getTile(x,y)->getPlayer();
				painter.drawText(QRect(x*tile.width(),y*tile.height(),tile.width(),tile.height()),"(" + QString::number(x) + "/" + QString::number(y) + ")\nPlayer: " + (playerNumber?QString::number(playerNumber):"none"));
			}
		}
	}
	painter.end();
	event->accept();
}

void BoardView::resizeEvent(QResizeEvent * event)
{
	int w = event->size().width() / board->getSize().width();
	int h = event->size().height() / board->getSize().height();
	fillCache(QSize(w,h));
	update();
	event->accept();
}

