#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"
#include "PlayerBiddingWidget.h"
#include "LeaderBoardWidget.h"



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    LeaderBoardWidget leaderBoardWidget(nullptr);
    PlayerBiddingWidget biddingWidget(nullptr);
    biddingWidget.setName("Kinski, Klaus");
    PlayerBiddingWidget biddingWidget2(nullptr);
    biddingWidget2.setName("Mandela, Nelson");
    leaderBoardWidget.setLayout(nullptr);
    leaderBoardWidget.addPlayer(&biddingWidget);
    leaderBoardWidget.addPlayer(&biddingWidget2);
    leaderBoardWidget.show();
//    biddingWidget.setLayout(nullptr);
    //biddingWidget.show();
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
