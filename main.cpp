#include "board.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    Board board(nullptr, 5, 8, 3);
    qDebug()<< "Hello" << board.getTile(0,0)->getWall(Direction::east);
	MainWindow w;
	w.show();    
	return a.exec();
}
