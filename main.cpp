#include <QApplication>
#include <QTranslator>
#include "keymappingview.h"
#include "mainwindow.h"
#include "UserBiddingWidget.h"
#include "LeaderBoardWidget.h"
#include "user.h"



int main(int argc, char *argv[])
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	QFont defaultFont = a.font();
	defaultFont.setPointSize(8);
	a.setFont(defaultFont);
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
	w.showMaximized();
	w.dumpObjectTree();
	return a.exec();
}

//Qt::WindowFlags flags;
//flags = Qt::Window;
//flags |= Qt::MSWindowsFixedSizeDialogHint;
//w.setWindowFlags(flags);
