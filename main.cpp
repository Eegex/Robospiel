#include "board.h"
#include "mainwindow.h"

#include <QApplication>



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    Board board(nullptr, 5, 8, 3);
	MainWindow w;
	w.show();    
	return a.exec();
}
