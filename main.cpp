#include <QApplication>
#include <QDir>
#include <QTranslator>
#include "keymappingview.h"
#include "mainwindow.h"
#include "UserBiddingWidget.h"
#include "LeaderBoardWidget.h"
#include "user.h"



int main(int argc, char *argv[])
{
	QDir saveOrdner(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	qDebug()<<saveOrdner;
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	//a.setPalette(QPalette(QColor(255,255,255),QColor(10,10,10),QColor(40,40,40),QColor(0,0,0),QColor(30,30,30),QColor(255,90,0),QColor(QColor(20,20,20))));
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
