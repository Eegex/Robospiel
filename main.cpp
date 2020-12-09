#include <QApplication>
#include <QTranslator>
#include "keymappingview.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	//BUG: random out_of_range once every ~20 launches...
	QApplication a(argc, argv);
//	QTranslator * translator = new QTranslator;
//    if(translator->load("Robospiel_de_DE"))
//	{
//		a.installTranslator(translator);
//	}
//	else
//	{
//		qDebug() << "Could not load translation file. Check the working directory!";
//	}
//    MainWindow w;
//    w.show();
    KeyMappingView* v=new KeyMappingView();
    v->show();
	return a.exec();
}
