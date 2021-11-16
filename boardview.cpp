#include "boardview.h"

/*!
 * \brief BoardView::BoardView Constructs a BoardView Object
 * \param parent the parent QWidget
 */
BoardView::BoardView(QWidget *parent) : QWidget(parent)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	grabGesture(Qt::PanGesture);
	grabGesture(Qt::SwipeGesture);
}

/*!
 * \brief Sets and connects the given board and  sets the layout, pawns and goal up.
 * \param newBoard The board to load
 */
void BoardView::setBoard(Board * newBoard)
{
	this->board = newBoard;
	connect(board,SIGNAL(boardChanged()),this,SLOT(update()));
	while(!playerWidgets.isEmpty())
	{
		delete playerWidgets.takeFirst();
	}
	delete goalwidget;
	goalwidget = new GoalWidget(QSize(20,20),board,this);
	goalwidget->show();
	for(int i = 0; i <board->players.length();i++)
	{
		addPlayer(i);
	}
	connect(board,&Board::playerMoved,this, [&](int playerNumber, int)
	{
		playerWidgets.at(playerNumber)->moveAnimated(tileToDesktopCoordinates(board->players.at(playerNumber)), board->players.at(playerNumber)->getPosition(), std::max(width(), height())*1.2);
	});
	connect(board,&Board::playerBeam,this, [&](int playerNumber)
	{
		playerWidgets.at(playerNumber)->moveAnimated(tileToDesktopCoordinates(board->players.at(playerNumber)), board->players.at(playerNumber)->getPosition(), std::max(width(), height())*1.2);
	});
	connect(board,&Board::paintPlayers,this, [&]()
	{
		for (int i =0;i<playerWidgets.length() ;i++ )
		{
			playerWidgets.at(i)->repaint();
		}
	}
	);
	connect(board,&Board::goalMoved,this,[&]()
	{
		goalwidget->move(tileToDesktopCoordinates(board->goal));
	});
	adjustSize();
}

/*!
 * \brief BoardView::setDebugOutputEnabled
 * \param set
 */
void BoardView::setDebugOutputEnabled(bool set)
{
	showDebugOutput = set;
}

/*!
 * \brief BoardView::resize
 * \param pixelPerTile
 */
void BoardView::resize(int pixelPerTile)
{
	tileSize = QSize(pixelPerTile,pixelPerTile);
	QWidget::resize(board->getSize() * pixelPerTile + QSize(10,10));
}

/*!
 * \brief BoardView::sizeHint
 * \return
 */
QSize BoardView::sizeHint() const
{
	return board->getSize() * 50 + QSize(10,10);
}

/*!
 * \brief BoardView::makeNewAll
 */
void BoardView::makeNewAll()
{
	board->makeNewBoard(board->getSize().width(), board->getSize().height(), board->players.length());
}

/*!
 * \brief BoardView::makeNewPlayers
 */
void BoardView::makeNewPlayers()
{
	board->makeNewPlayers(board->players.length());
}

/*!
 * \brief BoardView::makeNewSeeker
 */
void BoardView::makeNewSeeker()
{
	board->makeNewSeeker(false);
}

/*!
 * \brief BoardView::makeNewWalls
 */
void BoardView::makeNewWalls()
{
	board->makeNewWalls(board->getSize().width(), board->getSize().height());
}

void BoardView::makeNewTarget()
{
	board->makeNewGoal();
}

/*!
 * \brief BoardView::coordsToTile
 * \param p
 * \return
 */
Tile * BoardView::coordsToTile(QPoint p)
{
	int x = (p.x() - 5) / tileSize.width();
	int y = (p.y() - 5) / tileSize.height();
	return board->getTile(x,y);
}

/*!
 * \brief BoardView::setMapping
 * \param value
 */
void BoardView::setMapping(QVector<KeyMapping*> * value)
{
	mapping = value;
}

/*!
 * \brief BoardView::tileToDesktopCoordinates
 * \param tile
 * \return
 */
QPoint BoardView::tileToDesktopCoordinates(Tile* tile)
{
	double tileHeight = (height() - 10) / static_cast<double>(board->getSize().height());
	double tileWidth = (width() - 10) / static_cast<double>(board->getSize().width());
	return QPoint(5+(tile->getPosition().rx())*tileWidth,5+(tile->getPosition().ry())*tileHeight);
}

/*!
 * \brief BoardView::addPlayer
 * \param i
 * \return
 */
PlayerWidget * BoardView::addPlayer(int i)
{
	PlayerWidget * newPlayer = new PlayerWidget(QSize(50,50),i,board,this);
	playerWidgets.append(newPlayer);
	connect(newPlayer, &PlayerWidget::clicked,this, [&](int playerNumber)
	{
		emit activePlayerChanged(playerNumber);
	});
	connect(newPlayer, &PlayerWidget::reposition, this, [&](int playerNumber)
	{
		playerWidgets.at(playerNumber)->move(tileToDesktopCoordinates(board->players.at(playerNumber)));
		emit animationEnded();
	});
	newPlayer->move(tileToDesktopCoordinates(board->players[i]));
	newPlayer->show();
	newPlayer->raise();
	return newPlayer;
}

/*!
 * \brief BoardView::paintEvent
 * \param event
 */
void BoardView::paintEvent(QPaintEvent * event)
{
	QPainter painter;
	painter.begin(this);
	QPen debug(QColor(255,0,255));
	QPen gridPen(board->getGrid(),1,Qt::SolidLine,Qt::RoundCap);
	QPen player(QColor(0,0,0),2,Qt::SolidLine,Qt::RoundCap);
	double tileHeight = (height() - 10) / static_cast<double>(board->getSize().height());
	double tileWidth = (width() - 10) / static_cast<double>(board->getSize().width());
	QPen wallPen(board->getPrimary(),std::min(tileHeight, tileWidth)*0.1,Qt::SolidLine,Qt::SquareCap);
	tileSize = QSize(tileWidth,tileHeight);
	painter.setPen(gridPen);
	painter.setBrush(board->getBackground());
	painter.drawRect(this->rect() - QMargins(0,0,1,1));
	for(int y = 0; y < board->getSize().height(); y++)
	{
		for(int x = 0; x < board->getSize().width(); x++)
		{
			QRect tile(5+x*tileWidth,5+y*tileHeight,tileWidth,tileHeight);
			painter.drawRect(tile);
		}
	}
	painter.setPen(wallPen);
	for(int x = 0; x < board->getSize().width(); x++)
	{
		for(int y = 0; y < board->getSize().height(); y++)
		{
			int key = (board->getTile(x,y)->getWall(Direction::north)?static_cast<int>(Direction::north):0) |
					  (board->getTile(x,y)->getWall(Direction::east)?static_cast<int>(Direction::east):0) |
					  (board->getTile(x,y)->getWall(Direction::south)?static_cast<int>(Direction::south):0) |
					  (board->getTile(x,y)->getWall(Direction::west)?static_cast<int>(Direction::west):0);

			QRect tile(5+x*tileWidth,5+y*tileHeight,tileWidth,tileHeight);
			QRect outerTile(5+x*tileWidth-2,5+y*tileHeight-2,tileWidth+6,tileHeight+6); // falls man doch nochmal fancy walls will

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

/*!
 * \brief BoardView::resizeEvent
 * \param event
 */
void BoardView::resizeEvent(QResizeEvent * event)
{
	int w = event->size().width() / board->getSize().width();
	int h = event->size().height() / board->getSize().height();
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

/*!
 * \brief BoardView::mousePressEvent
 * \param event
 */
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

/*!
 * \brief BoardView::mouseMoveEvent
 * \param event
 */
void BoardView::mouseMoveEvent(QMouseEvent * event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		QPoint moved = event->pos() - mouseStart;
		if(moved.x() > 100)
		{
			if(Qt::Key_F32 != lastKey) // FIXME: better solution
			{
				lastKey = Qt::Key_F32;
				handleKeyPress(Qt::Key_F32);
				return;
			}
			//qDebug() << "east";
		}
		else if(moved.x() < -100)
		{
			if(Qt::Key_F34 != lastKey) // FIXME: better solution
			{
				lastKey = Qt::Key_F34;
				handleKeyPress(Qt::Key_F34);
				return;
			}
			//qDebug() << "west";
		}
		else if(moved.y() > 100)
		{
			if(Qt::Key_F33 != lastKey) // FIXME: better solution
			{
				lastKey = Qt::Key_F33;
				handleKeyPress(Qt::Key_F33);
				return;
			}
			//qDebug() << "south";
		}
		else if(moved.y() < -100)
		{
			if(Qt::Key_F31 != lastKey) // FIXME: better solution
			{
				lastKey = Qt::Key_F31;
				handleKeyPress(Qt::Key_F31);
				return;
			}
			//qDebug() << "north";
		}
	}
	else
	{
		Tile * t = coordsToTile(event->pos());
		if(t)
		{
			handleKeyPress(Qt::Key_F35);
		}
	}
}

/*!
 * \brief BoardView::keyPressEvent
 * \param event
 */
void BoardView::keyPressEvent(QKeyEvent * event)
{
	handleKeyPress(event->key());
	QWidget::keyPressEvent(event);
}

/*!
 * \brief BoardView::handleKeyPress
 * \param key
 */
void BoardView::handleKeyPress(int key)
{
	for(const KeyMapping * k:qAsConst(*mapping))
	{
		if(*k == key)
		{
			emit action(k->getAction(), "");
			return;
		}
	}
}

/*!
 * \brief BoardView::event
 * \param event
 * \return
 */
bool BoardView::event(QEvent * event)
{
	if(event->type() == QEvent::Gesture)
	{
		QGestureEvent * gestureEvent = dynamic_cast<QGestureEvent*>(event);
		if(gestureEvent)
		{
			gestureEvent->accept();
			QGesture * g = gestureEvent->gesture(Qt::SwipeGesture);
			QSwipeGesture * sg = dynamic_cast<QSwipeGesture*>(g);
			if(sg)
			{
				if (sg->state() == Qt::GestureFinished)
				{
					if(sg->verticalDirection() == QSwipeGesture::Up)
					{
						handleKeyPress(Qt::Key_F31);
					}
					else if(sg->horizontalDirection() == QSwipeGesture::Right)
					{
						handleKeyPress(Qt::Key_F32);
					}
					else if(sg->verticalDirection() == QSwipeGesture::Down)
					{
						handleKeyPress(Qt::Key_F33);
					}
					else if(sg->horizontalDirection() == QSwipeGesture::Left)
					{
						handleKeyPress(Qt::Key_F34);
					}
				}
			}
		}
	}
	return QWidget::event(event);
}

/*!
 * \brief BoardView::getBoard
 * \return
 */
Board * BoardView::getBoard()
{
	return board;
}
