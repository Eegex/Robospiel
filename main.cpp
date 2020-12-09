#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"
#include "PlayerBiddingWidget.h"
#include "LeaderBoardWidget.h"
#include "user.h"



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator * translator = new QTranslator;
	if(translator->load("Robospiel_de_DE"))
	{
		a.installTranslator(translator);
	}
	else
	{
		qDebug() << "Could not load translation file. Check the working directory!";
	}
	MainWindow w;
	w.show();
	return a.exec();
}
