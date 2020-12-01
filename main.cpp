#include <QApplication>
#include "mainwindow.h"
#include "board.h"
#include "boardview.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    Board board(16, 16, 5);
	BoardView view;
	view.setBoard(&board);
	view.show();
	MainWindow w;
	w.show();
	return a.exec();
}
