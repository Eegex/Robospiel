#include "boardview.h"

BoardView::BoardView(QWidget *parent) : QWidget(parent)
{
	mapping.append(KeyMapping(PlayerAction::movePlayerNorth,{Qt::Key::Key_W,Qt::Key::Key_Up}));
	mapping.append(KeyMapping(PlayerAction::movePlayerEast,{Qt::Key::Key_D,Qt::Key::Key_Right}));
	mapping.append(KeyMapping(PlayerAction::movePlayerSouth,{Qt::Key::Key_S,Qt::Key::Key_Down}));
	mapping.append(KeyMapping(PlayerAction::movePlayerWest,{Qt::Key::Key_A,Qt::Key::Key_Left}));
	mapping.append(KeyMapping(PlayerAction::revert,Qt::Key::Key_R));
	mapping.append(KeyMapping(PlayerAction::giveUp,Qt::Key::Key_Q));
	mapping.append(KeyMapping(PlayerAction::enterBidding,Qt::Key::Key_Space));
	mapping.append(KeyMapping(PlayerAction::clearBidding,Qt::Key::Key_Backspace));
	mapping.append(KeyMapping(PlayerAction::sendBidding,Qt::Key::Key_Return));
	setMouseTracking(true);
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
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

void BoardView::resize(int pixelPerTile)
{
	tileSize = QSize(pixelPerTile,pixelPerTile);
	QWidget::resize(board->getSize() * pixelPerTile + QSize(10,10));
}

QSize BoardView::sizeHint() const
{
	return board->getSize() * 50 + QSize(10,10);
}

void BoardView::fillCache(QSize tileSize)
{
	if(!cachedPaintig)
	{
		return;
	}
	cache.clear();
	this->tileSize = tileSize;
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

Tile * BoardView::coordsToTile(QPoint p)
{
	int x = (p.x() - 5) / tileSize.width();
	int y = (p.y() - 5) / tileSize.height();
	if(x < board->getSize().width() && y < board->getSize().height())
	{
		return board->getTile(x,y);
	}
	return nullptr;
}

void BoardView::paintEvent(QPaintEvent * event)
{
	QPainter painter;
	painter.begin(this);
//	painter.setRenderHint(QPainter::Antialiasing);
	QPen debug(QColor(255,0,255));
	QPen grid(QColor(0,80,0),1,Qt::SolidLine,Qt::RoundCap);
	QPen wall(QColor(0,120,0),2,Qt::SolidLine,Qt::RoundCap);
	double tileHeight = (height() - 10) / static_cast<double>(board->getSize().height());
	double tileWidth = (width() - 10) / static_cast<double>(board->getSize().width());
	tileSize = QSize(tileWidth,tileHeight);
	if(!cachedPaintig)
	{
		painter.setPen(grid);
		painter.setBrush(QColor(20,20,20));
		painter.drawRect(this->rect());
		for(int y = 0; y < board->getSize().height(); y++)
		{
			for(int x = 0; x < board->getSize().width(); x++)
			{
				QRect tile(5+x*tileWidth,5+y*tileHeight,tileWidth,tileHeight);
				painter.drawRect(tile);
			}
		}
		painter.setPen(wall);
	}
	for(int x = 0; x < board->getSize().width(); x++)
	{
		for(int y = 0; y < board->getSize().height(); y++)
		{
			int key = (board->getTile(x,y)->getWall(Direction::north)?static_cast<int>(Direction::north):0) |
					  (board->getTile(x,y)->getWall(Direction::east)?static_cast<int>(Direction::east):0) |
					  (board->getTile(x,y)->getWall(Direction::south)?static_cast<int>(Direction::south):0) |
					  (board->getTile(x,y)->getWall(Direction::west)?static_cast<int>(Direction::west):0);
			if(cachedPaintig)
			{
				QPixmap tile;
				if(cache.find(QString::number(key,2),&tile))
				{
					painter.drawPixmap(5+x*tile.width(),5+y*tile.height(),tile);
				}
			}
			else
			{
				QRect tile(5+x*tileWidth-2,5+y*tileHeight-2,tileWidth+6,tileHeight+6);
				if(key & static_cast<int>(Direction::north))
				{
					painter.drawLine(tile.topLeft(),tile.topRight());
				}
				if(key & static_cast<int>(Direction::east))
				{
					painter.drawLine(tile.topRight(),tile.bottomRight());
				}
				if(key & static_cast<int>(Direction::south))
				{
					painter.drawLine(tile.bottomRight(),tile.bottomLeft());
				}
				if(key & static_cast<int>(Direction::west))
				{
					painter.drawLine(tile.bottomLeft(),tile.topLeft());
				}
			}
		}
	}
	if(showDebugOutput)
	{
		painter.setPen(debug);
		for(int x = 0; x < board->getSize().width(); x++)
		{
			for(int y = 0; y < board->getSize().height(); y++)
			{
				QRect tile(x*tileWidth,y*tileHeight,tileWidth,tileHeight);
				int playerNumber = board->getTile(x,y)->getPlayer();
				painter.drawText(QRect(8+x*tile.width(),8+y*tile.height(),tile.width(),tile.height()),"(" + QString::number(x) + "/" + QString::number(y) + ")\nPlayer: " + (playerNumber?QString::number(playerNumber):"none"));
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

void BoardView::mousePressEvent(QMouseEvent * event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		mouseStart = event->pos();
		Tile * t = coordsToTile(event->pos());
		if(t)
		{
			emit tileClicked(t);
		}
	}
}

void BoardView::mouseMoveEvent(QMouseEvent * event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		QPoint moved = event->pos() - mouseStart;
		if(moved.x() > 100)
		{
			emit swipe(Direction::east);
		}
		else if(moved.x() < -100)
		{
			emit swipe(Direction::west);
		}
		else if(moved.y() > 100)
		{
			emit swipe(Direction::south);
		}
		else if(moved.y() < -100)
		{
			emit swipe(Direction::north);
		}
	}
	else
	{
		Tile * t = coordsToTile(event->pos());
		if(t)
		{
			emit tileHovered(t);
		}
	}
}

void BoardView::keyPressEvent(QKeyEvent * event)
{
	for(const KeyMapping & k:mapping)
	{
		if(k == event->key())
		{
			emit action(k.getAction());
			return;
		}
	}
	QWidget::keyPressEvent(event);
}
