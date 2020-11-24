

#include <QApplication>
#include "mainwindow.h"
#include "board.h"
#include "boardview.h"



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    Board board(nullptr, 16, 16, 3);
	BoardView view(nullptr);
	view.setBoard(&board);
	view.show();
	MainWindow w;
	w.show();    
	return a.exec();
}
