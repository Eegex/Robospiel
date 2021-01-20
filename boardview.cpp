#include "boardview.h"

BoardView::BoardView(QWidget *parent) : QWidget(parent)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	fillCache(QSize(50,50));
	//connect(this, &BoardView::swipe, this, &BoardView::callMoveActivePlayer);
	//connect(this, &BoardView::tileClicked, this, &BoardView::callChangeActivePlayer);
	//connect(this, &BoardView::action, this, &BoardView::translateMapping);
}

void BoardView::setBoard(Board * b)
{
	board = b;
	connect(board,SIGNAL(boardChanged()),this,SLOT(update()));
	for(int i = 0; i <board->players.length();i++)
	{
		playerWidgets.append(new PlayerWidget(QSize(50,50),i,board,this));
		connect(playerWidgets.back(), &PlayerWidget::clicked,this, [&](int playerNumber){emit activePlayerChanged(playerNumber);} );
		connect(playerWidgets.back(), &PlayerWidget::reposition, this, [&](int playerNumber){
			playerWidgets.at(playerNumber)->move(tileToDesktopCoordinates(board->players.at(playerNumber)));
            if(goalWaitingToBeEmitted)
            {
                goalWaitingToBeEmitted=false;
                emit lastAnimationEnded();
            }
		});

	}
	goalwidget = new GoalWidget(QSize(20,20),board,this);
    connect(board,&Board::playerMoved,this, [&](int playerNumber, bool goalHit){
        if(goalHit)
        {
            goalWaitingToBeEmitted=true;
        }
        playerWidgets.at(playerNumber)->moveAnimated(tileToDesktopCoordinates(board->players.at(playerNumber)), board->players.at(playerNumber)->getPosition(), std::max(width(), height())*0.2);
    });
	connect(board,&Board::goalMoved,this, [&](){goalwidget->move(tileToDesktopCoordinates(board->goal));});
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
	QPen gridPen(grid,1);
	QPen primaryPen(primary,10);
	QPainter painter;
	QRect tileRect(0,0,tileSize.width(),tileSize.height());
	QPixmap tile(tileSize);
	for(int i = 0;i < 16; i++)
	{
		painter.begin(&tile);
		painter.setPen(gridPen);
		painter.setBrush(background);
		painter.drawRect(tileRect);
		painter.setPen(primaryPen);
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
	return board->getTile(x,y);
}

void BoardView::setMapping(QVector<KeyMapping*> * value)
{
	mapping = value;
}

void BoardView::updateColors(QColor b, QColor w, QColor g)
{
	background = b;
	primary = w;
	grid = g;
	update();
}

QPoint BoardView::tileToDesktopCoordinates(Tile* tile)
{
	double tileHeight = (height() - 10) / static_cast<double>(board->getSize().height());
	double tileWidth = (width() - 10) / static_cast<double>(board->getSize().width());
	return QPoint(5+(tile->getPosition().rx())*tileWidth,5+(tile->getPosition().ry())*tileHeight);
}


void BoardView::paintEvent(QPaintEvent * event)
{
	QPainter painter;
	painter.begin(this);
	//painter.setRenderHint(QPainter::Antialiasing);
	QPen debug(QColor(255,0,255));
	QPen gridPen(grid,1,Qt::SolidLine,Qt::RoundCap);
	QPen wallPen(primary,2,Qt::SolidLine,Qt::RoundCap);
	QPen player(QColor(0,0,0),2,Qt::SolidLine,Qt::RoundCap);

	double tileHeight = (height() - 10) / static_cast<double>(board->getSize().height());
	double tileWidth = (width() - 10) / static_cast<double>(board->getSize().width());
	tileSize = QSize(tileWidth,tileHeight);
	if(!cachedPaintig)
	{
		painter.setPen(gridPen);
		painter.setBrush(background);
		painter.drawRect(this->rect());
		for(int y = 0; y < board->getSize().height(); y++)
		{
			for(int x = 0; x < board->getSize().width(); x++)
			{
				QRect tile(5+x*tileWidth,5+y*tileHeight,tileWidth,tileHeight);
				painter.drawRect(tile);
			}
		}
		//		painter.setPen(player);
		//		int playerNum = board->players.length();
		//		double stepSize = 359/playerNum;
		//		QColor color;
		//		for(int i = 0; i <board->players.length();i++)
		//		{
		//			color.setHsv(i*stepSize,200,200);
		//			painter.setBrush(color);
		//			if(i == board->seeker)
		//			{
		//				//draw the goal
		//			painter.drawEllipse();
		//			}
		//			Tile* player = board->players.at(i);
		//			painter.drawEllipse(QPoint(5+(player->getPosition().rx()+1)*tileWidth - tileWidth/2,5+(player->getPosition().ry()+1)*tileHeight - tileHeight/2), (int) tileWidth/3, (int) tileWidth/3);
		//		}
		painter.setPen(wallPen);
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
				painter.drawText(QRect(8+x*tile.width(),8+y*tile.height(),tile.width(),tile.height()),"(" + QString::number(x) + "/" + QString::number(y) + ")\nPlayer: " + (playerNumber+1?QString::number(playerNumber):"none"));
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
    currentEvent=*event;
    QTimer::singleShot(50, this, [=]()->void{
        //update players
        for(int i = 0; i <board->players.length();i++)
        {
            playerWidgets.at(i)->setFixedSize(w,h);

            //calculate all future animations
            QVector<QPoint> targets = playerWidgets.at(i)->getTargets();
            for(int j=0; j<targets.size(); j++)
            {
                 targets.replace(j,tileToDesktopCoordinates(board->getTile(targets.at(j).x(), targets.at(j).y())));
            }

            //calculate new position
            QPoint newPosition = tileToDesktopCoordinates(board->players.at(i));
            double factorX = 1;
            double factorY = 1;
            if(!currentEvent.oldSize().isEmpty() && playerWidgets.at(i)->getInAnimation())
            {
                factorX = currentEvent.size().width()*1.0/currentEvent.oldSize().width();
                factorY = currentEvent.size().height()*1.0/currentEvent.oldSize().height();
                newPosition=playerWidgets.at(i)->pos();
                double newX=newPosition.x()*factorX;
                double newY=newPosition.y()*factorY;

                //cannot be outside of the board
                newPosition.rx()=round(std::min(std::max(newX, tileToDesktopCoordinates(board->getTile(0, 0)).x()*1.0), tileToDesktopCoordinates(board->getTile(board->getSize().width()-1, 0)).x()*1.0));
                newPosition.ry()=round(std::min(std::max(newY, tileToDesktopCoordinates(board->getTile(0, 0)).y()*1.0), tileToDesktopCoordinates(board->getTile(0, board->getSize().height()-1)).y()*1.0));
            }


            //apply
            if(!playerWidgets.at(i)->resizeWhileAnimation(targets, newPosition, factorX, factorY))
            {
                playerWidgets.at(i)->move(newPosition);
            }
        }
    });

	goalwidget->move(tileToDesktopCoordinates(board->goal));
	goalwidget->setFixedSize(w,h);
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
			qDebug() << "east";
		}
		else if(moved.x() < -100)
		{
			emit swipe(Direction::west);
			qDebug() << "west";
		}
		else if(moved.y() > 100)
		{
			emit swipe(Direction::south);
			qDebug() << "south";
		}
		else if(moved.y() < -100)
		{
			emit swipe(Direction::north);
			qDebug() << "north";
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
	for(const KeyMapping * k:*mapping)
	{
		if(*k == event->key())
		{
			emit action(k->getAction(), "");
			return;
		}
	}
	QWidget::keyPressEvent(event);
}

//void BoardView::callMoveActivePlayer(Direction d)
//{
//	board->moveActivePlayer(d);
//	update();
//}
//void BoardView::callChangeActivePlayer(Tile * t)
//{
//	board->changeActivePlayer(t);
//	update();
//}
//void BoardView::translateMapping(PlayerAction action)
//{
//	Direction d = Direction::north;
//	switch (action)
//	{
//	case movePlayerNorth:
//		d = Direction::north;
//		break;
//	case movePlayerEast:
//		 d = Direction::east;
//		break;
//	case movePlayerWest:
//		d = Direction::west;
//		break;
//	case movePlayerSouth:
//		 d = Direction::south;
//		break;
//	}
//	callMoveActivePlayer(d);
//}
