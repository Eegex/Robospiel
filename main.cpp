

#include <QApplication>
#include "mainwindow.h"
#include "board.h"
#include "boardview.h"
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
    Board board(16, 16, 5);
	BoardView view;
	view.setBoard(&board);
	view.show();
	MainWindow w;
	w.show();
	return a.exec();
}
